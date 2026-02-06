#pragma once

#include <Processors/TTL/ITTLAlgorithm.h>
#include <Interpreters/Aggregator.h>
#include <Interpreters/ExpressionActions.h>
#include <Core/SortDescription.h>
#include <Storages/MergeTree/MergeTreeData.h>

namespace DB
{

/// Aggregates rows according to 'TTL expr GROUP BY key' description.
/// Aggregation key must be the prefix of the sorting key.
class TTLAggregationAlgorithm final : public ITTLAlgorithm
{
public:
    TTLAggregationAlgorithm(
        const TTLExpressions & ttl_expressions_,
        const TTLDescription & description_,
        const TTLInfo & old_ttl_info_,
        time_t current_time_,
        bool force_,
        const Block & header_,
        const MergeTreeData & storage_);

    void execute(Block & block) override;
    void finalize(const MutableDataPartPtr & data_part) const override;

private:
    // Calculate aggregates of aggregate_columns into aggregation_result
    void calculateAggregates(const MutableColumns & aggregate_columns, size_t start_pos, size_t length);

    /// Finalize aggregation_result into result_columns
    void finalizeAggregates(MutableColumns & result_columns);

    const Block header;
    std::unique_ptr<Aggregator> aggregator;
    Row current_key_value;
    AggregatedDataVariants aggregation_result;
    ColumnRawPtrs key_columns;
    Aggregator::AggregateColumns columns_for_aggregator;
    bool no_more_keys = false;

    /// Columns that are modified by SET and required by the sorting key expression.
    /// When SET changes these columns, the sorting key may change, violating sort order.
    NameSet sorting_key_columns_to_clamp;

    /// Sorting key expression (for computing sort key from source columns)
    ExpressionActionsPtr sorting_key_expression;

    /// Sort description for lexicographic comparison of sorting key columns
    SortDescription sort_description;

    /// First and last row's values for columns in sorting_key_columns_to_clamp
    /// (first row = min sort key, last row = max sort key within the group)
    std::unordered_map<String, Field> first_row_values;
    std::unordered_map<String, Field> last_row_values;
    bool has_first_row = false;

    /// For cross-block sort order verification
    Columns last_block_sort_key_columns;
};

}
