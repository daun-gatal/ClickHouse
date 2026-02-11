#include <unordered_set>
#include <Planner/CollectMaterializedCTE.h>

#include <Analyzer/TableNode.h>
#include <Analyzer/traverseQueryTree.h>
#include <Common/Logger.h>
#include <Common/logger_useful.h>

namespace DB
{

TableHolderToCTEMap collectMaterializedCTEs(const QueryTreeNodePtr & node, const SelectQueryOptions & select_query_options)
{
    if (select_query_options.is_subquery)
        return {};

    TableHolderToCTEMap materialized_ctes;

    traverseQueryTree(node, ExceptSubqueries{},
    [&](const QueryTreeNodePtr & current_node)
    {
        if (auto * table_node = current_node->as<TableNode>())
        {
            if (table_node->isMaterializedCTE())
            {
                auto [_, inserted] = materialized_ctes.emplace(table_node->getTemporaryTableHolder().get(), current_node);
                LOG_DEBUG(getLogger("collectMaterializedCTEs"), "Found materialized CTE (inserted: {}):\n{}", inserted, table_node->dumpTree());
            }
        }
    });

    return materialized_ctes;
}

}
