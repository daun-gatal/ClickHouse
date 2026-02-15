#include <Planner/AnalyzeExpression.h>

#include <Analyzer/ColumnNode.h>
#include <Analyzer/QueryNode.h>
#include <Analyzer/QueryTreeBuilder.h>
#include <Analyzer/Resolve/QueryAnalyzer.h>
#include <Analyzer/TableNode.h>
#include <Interpreters/Context.h>
#include <Parsers/ASTExpressionList.h>
#include <Planner/CollectSets.h>
#include <Planner/CollectTableExpressionData.h>
#include <Planner/PlannerContext.h>
#include <Planner/Utils.h>
#include <Storages/ColumnsDescription.h>
#include <Storages/StorageDummy.h>


namespace DB
{

ActionsDAG analyzeExpressionToActionsDAG(
    const ASTPtr & expression_ast,
    const NamesAndTypesList & available_columns,
    const ContextPtr & context,
    bool add_aliases)
{
    /// Ensure the AST is an expression list, because QueryNode projection expects a ListNode.
    ASTPtr expr_list_ast = expression_ast;
    if (!expr_list_ast->as<ASTExpressionList>())
    {
        auto wrapper = make_intrusive<ASTExpressionList>();
        wrapper->children.push_back(expr_list_ast);
        expr_list_ast = wrapper;
    }

    const auto & ast_children = expr_list_ast->as<ASTExpressionList &>().children;

    /// Handle empty expression list (e.g., ORDER BY tuple() produces no key columns,
    /// or missing PARTITION BY produces an empty partition key).
    if (ast_children.empty())
    {
        ActionsDAG dag;
        for (const auto & col : available_columns)
            dag.addInput(col.name, col.type);
        return dag;
    }

    /// Collect AST column names to use for output renaming, so that callers
    /// that rely on ast->getColumnName() (e.g. findInOutputs) work correctly.
    std::vector<String> ast_column_names;
    ast_column_names.reserve(ast_children.size());
    for (const auto & child : ast_children)
        ast_column_names.push_back(child->getColumnName());

    auto execution_context = Context::createCopy(context);

    ColumnsDescription columns_description(available_columns);
    auto storage = std::make_shared<StorageDummy>(StorageID{"dummy", "dummy"}, columns_description);
    QueryTreeNodePtr fake_table_expression = std::make_shared<TableNode>(storage, execution_context);

    auto global_planner_context = std::make_shared<GlobalPlannerContext>(nullptr, nullptr, FiltersForTableExpressionMap{});
    auto planner_context = std::make_shared<PlannerContext>(execution_context, global_planner_context, SelectQueryOptions{});

    QueryAnalyzer analyzer(/* only_analyze */ true);

    auto query_node = std::make_shared<QueryNode>(execution_context);

    auto expression_list = buildQueryTree(expr_list_ast, execution_context);

    query_node->getProjectionNode() = expression_list;
    query_node->getJoinTree() = fake_table_expression;

    QueryTreeNodePtr query_tree = query_node;
    analyzer.resolve(query_tree, nullptr, execution_context);

    query_node = std::static_pointer_cast<QueryNode>(query_tree);
    expression_list = query_node->getProjectionNode();

    collectSourceColumns(expression_list, planner_context, false);
    collectSets(expression_list, *planner_context);

    ColumnNodePtrWithHashSet empty_correlated_columns_set;
    auto [actions, _] = buildActionsDAGFromExpressionNode(
        expression_list,
        {},
        planner_context,
        empty_correlated_columns_set);

    if (add_aliases)
    {
        /// Project to only the expression columns, renamed to match AST column names.
        auto & outputs = actions.getOutputs();
        NamesWithAliases rename_pairs;
        rename_pairs.reserve(outputs.size());

        for (size_t i = 0; i != outputs.size(); ++i)
            rename_pairs.emplace_back(outputs[i]->result_name, ast_column_names[i]);

        actions.project(rename_pairs);
    }
    else
    {
        /// Rename expression outputs to match AST column names, so that callers
        /// using ast->getColumnName() can find them in the DAG.
        auto & outputs = actions.getOutputs();
        for (size_t i = 0; i < outputs.size(); ++i)
        {
            if (outputs[i]->result_name != ast_column_names[i])
                outputs[i] = &actions.addAlias(*outputs[i], ast_column_names[i]);
        }

        /// Add source columns to outputs to match ExpressionAnalyzer::getActions(false) behavior.
        /// The old code included all source columns in the output; buildActionsDAGFromExpressionNode
        /// only outputs the expression results.
        std::vector<const ActionsDAG::Node *> inputs_to_add;
        for (const auto * input : actions.getInputs())
        {
            bool already_in_outputs = false;
            for (const auto * output : outputs)
            {
                if (output == input)
                {
                    already_in_outputs = true;
                    break;
                }
            }
            if (!already_in_outputs)
                inputs_to_add.push_back(input);
        }
        for (const auto * input : inputs_to_add)
            outputs.push_back(input);
    }

    return std::move(actions);
}

ExpressionActionsPtr analyzeExpressionToActions(
    const ASTPtr & expression_ast,
    const NamesAndTypesList & available_columns,
    const ContextPtr & context,
    bool add_aliases)
{
    auto dag = analyzeExpressionToActionsDAG(expression_ast, available_columns, context, add_aliases);
    return std::make_shared<ExpressionActions>(std::move(dag), ExpressionActionsSettings(context));
}

}
