#include <Planner/CollectMaterializedCTE.h>

#include <Analyzer/TableNode.h>
#include <Analyzer/traverseQueryTree.h>

namespace DB
{

struct MaterializedCTE
{
    QueryTreeNodePtr table_node;
    size_t level;
};

using TableHolderToCTEMap = std::unordered_map<const TemporaryTableHolder *, MaterializedCTE>;

OrderedMaterializedCTEs collectMaterializedCTEs(const QueryTreeNodePtr & node, const SelectQueryOptions & select_query_options)
{
    if (select_query_options.is_subquery)
        return {};

    TableHolderToCTEMap materialized_ctes;
    OrderedMaterializedCTEs ctes_by_level;

    size_t level = 0;
    size_t max_level = 0;
    traverseQueryTree(node, Everything{},
    [&](const QueryTreeNodePtr & current_node)
    {
        if (auto * table_node = current_node->as<TableNode>())
        {
            if (table_node->isMaterializedCTE())
            {
                auto [it, _] = materialized_ctes.emplace(table_node->getTemporaryTableHolder().get(), MaterializedCTE{current_node, level});

                it->second.level = std::max(it->second.level, level);
                max_level = std::max(max_level, level);

                ++level;
            }
        }
    },
    [&level](const QueryTreeNodePtr & current_node)
    {
        if (auto * table_node = current_node->as<TableNode>())
        {
            if (table_node->isMaterializedCTE())
                --level;
        }
    });

    if (materialized_ctes.empty())
        return ctes_by_level;

    ctes_by_level.resize(max_level + 1);
    for (const auto & [_, future_table] : materialized_ctes)
    {
        /// Deepest materialized CTEs should be executed first, because CTEs with lower levels depend on them.
        ctes_by_level[future_table.level].push_back(future_table.table_node);
    }

    return ctes_by_level;
}

}
