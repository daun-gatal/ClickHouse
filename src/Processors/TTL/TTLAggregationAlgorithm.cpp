#include <Core/Settings.h>
#include <Core/SortDescription.h>
#include <DataTypes/DataTypeLowCardinality.h>
#include <Interpreters/Context.h>
#include <Interpreters/ExpressionActions.h>
#include <Processors/TTL/TTLAggregationAlgorithm.h>
#include <Storages/KeyDescription.h>
#include <Storages/StorageInMemoryMetadata.h>

#include <unordered_set>

namespace DB
{

namespace ErrorCodes
{
    extern const int BAD_TTL_EXPRESSION;
}

namespace Setting
{
    extern const SettingsBool compile_aggregate_expressions;
    extern const SettingsBool empty_result_for_aggregation_by_empty_set;
    extern const SettingsBool enable_software_prefetch_in_aggregation;
    extern const SettingsOverflowModeGroupBy group_by_overflow_mode;
    extern const SettingsNonZeroUInt64 max_block_size;
    extern const SettingsUInt64 max_bytes_before_external_group_by;
    extern const SettingsDouble max_bytes_ratio_before_external_group_by;
    extern const SettingsUInt64 max_rows_to_group_by;
    extern const SettingsMaxThreads max_threads;
    extern const SettingsNonZeroUInt64 min_chunk_bytes_for_parallel_parsing;
    extern const SettingsUInt64 min_count_to_compile_aggregate_expression;
    extern const SettingsUInt64 min_free_disk_space_for_temporary_data;
    extern const SettingsBool optimize_group_by_constant_keys;
    extern const SettingsBool enable_producing_buckets_out_of_order_in_aggregation;
    extern const SettingsBool serialize_string_in_memory_with_zero_byte;
}

TTLAggregationAlgorithm::TTLAggregationAlgorithm(
    const TTLExpressions & ttl_expressions_,
    const TTLDescription & description_,
    const TTLInfo & old_ttl_info_,
    time_t current_time_,
    bool force_,
    const Block & header_,
    const MergeTreeData & storage_)
    : ITTLAlgorithm(ttl_expressions_, description_, old_ttl_info_, current_time_, force_)
    , header(header_)
{
    current_key_value.resize(description.group_by_keys.size());

    const auto & keys = description.group_by_keys;

    key_columns.resize(description.group_by_keys.size());
    AggregateDescriptions aggregates = description.aggregate_descriptions;

    columns_for_aggregator.resize(description.aggregate_descriptions.size());
    const Settings & settings = storage_.getContext()->getSettingsRef();

    Aggregator::Params params(
        keys,
        aggregates,
        /*overflow_row_=*/false,
        settings[Setting::max_rows_to_group_by],
        settings[Setting::group_by_overflow_mode],
        /*group_by_two_level_threshold*/ 0,
        /*group_by_two_level_threshold_bytes*/ 0,
        Aggregator::Params::getMaxBytesBeforeExternalGroupBy(
            settings[Setting::max_bytes_before_external_group_by], settings[Setting::max_bytes_ratio_before_external_group_by]),
        settings[Setting::empty_result_for_aggregation_by_empty_set],
        storage_.getContext()->getTempDataOnDisk(),
        settings[Setting::max_threads],
        settings[Setting::min_free_disk_space_for_temporary_data],
        settings[Setting::compile_aggregate_expressions],
        settings[Setting::min_count_to_compile_aggregate_expression],
        settings[Setting::max_block_size],
        settings[Setting::enable_software_prefetch_in_aggregation],
        /*only_merge=*/false,
        settings[Setting::optimize_group_by_constant_keys],
        static_cast<float>(settings[Setting::min_chunk_bytes_for_parallel_parsing]),
        /*stats_collecting_params_=*/{},
        settings[Setting::enable_producing_buckets_out_of_order_in_aggregation],
        settings[Setting::serialize_string_in_memory_with_zero_byte]);

    aggregator = std::make_unique<Aggregator>(header, params);

    /// Determine which SET columns affect the sorting key.
    /// If SET modifies a column that the sorting key expression depends on,
    /// the result row's sorting key may fall outside the original group's range,
    /// violating the sort invariant. We detect this and clamp such columns.
    auto metadata = storage_.getInMemoryMetadataPtr();
    if (metadata->hasSortingKey())
    {
        const auto & sorting_key = metadata->getSortingKey();
        sorting_key_expression = sorting_key.expression;

        /// Build sort description from sorting key
        for (size_t i = 0; i < sorting_key.column_names.size(); ++i)
        {
            int direction = (sorting_key.reverse_flags.empty() || !sorting_key.reverse_flags[i]) ? 1 : -1;
            sort_description.push_back(SortColumnDescription(sorting_key.column_names[i], direction));
        }

        /// Columns required as inputs for the sorting key expression
        Names required_columns = sorting_key.expression->getRequiredColumns();
        NameSet required_columns_set(required_columns.begin(), required_columns.end());

        /// Find SET columns that are also required by the sorting key
        for (const auto & set_part : description.set_parts)
        {
            if (required_columns_set.contains(set_part.column_name))
                sorting_key_columns_to_clamp.insert(set_part.column_name);
        }

        /// If no SET columns affect the sorting key, disable clamping
        if (sorting_key_columns_to_clamp.empty())
        {
            sorting_key_expression = nullptr;
            sort_description.clear();
        }
    }

    if (isMaxTTLExpired())
        new_ttl_info.ttl_finished = true;
}

void TTLAggregationAlgorithm::execute(Block & block)
{

    bool some_rows_were_aggregated = false;
    MutableColumns result_columns = header.cloneEmptyColumns();

    if (block.empty()) /// Empty block -- no more data, but we may still have some accumulated rows
    {
        if (!aggregation_result.empty()) /// Still have some aggregated data, let's update TTL
        {
            finalizeAggregates(result_columns);
            some_rows_were_aggregated = true;
        }
        else /// No block, all aggregated, just finish
        {
            return;
        }
    }
    else
    {
        const auto & column_names = header.getNames();
        MutableColumns aggregate_columns = header.cloneEmptyColumns();

        auto ttl_column = executeExpressionAndGetColumn(ttl_expressions.expression, block, description.result_column);
        auto where_column = executeExpressionAndGetColumn(ttl_expressions.where_expression, block, description.where_result_column);

        size_t rows_aggregated = 0;
        size_t current_key_start = 0;
        size_t rows_with_current_key = 0;

        for (size_t i = 0; i < block.rows(); ++i)
        {
            Int64 cur_ttl = getTimestampByIndex(ttl_column.get(), i);
            bool where_filter_passed = !where_column || where_column->getBool(i);
            bool ttl_expired = isTTLExpired(cur_ttl) && where_filter_passed;

            bool same_as_current = true;
            for (size_t j = 0; j < description.group_by_keys.size(); ++j)
            {
                const String & key_column = description.group_by_keys[j];
                const IColumn * values_column = block.getByName(key_column).column.get();
                if (!same_as_current || (*values_column)[i] != current_key_value[j])
                {
                    values_column->get(i, current_key_value[j]);
                    same_as_current = false;
                }
            }

            /// We are observing the row with new the aggregation key.
            /// In this case we definitely need to finish the current aggregation for the previuos key and
            /// write results to `result_columns`.
            const bool observing_new_key = !same_as_current;
            /// We are observing the row with the same aggregation key, but TTL is not expired anymore.
            /// In this case we need to finish aggregation here. The current row has to be written as is.
            const bool no_new_rows_to_aggregate_within_the_same_key = same_as_current && !ttl_expired;
            /// The aggregation for this aggregation key is done.
            const bool need_to_flush_aggregation_state = observing_new_key || no_new_rows_to_aggregate_within_the_same_key;

            if (need_to_flush_aggregation_state)
            {
                if (rows_with_current_key)
                {
                    some_rows_were_aggregated = true;
                    calculateAggregates(aggregate_columns, current_key_start, rows_with_current_key);
                }
                finalizeAggregates(result_columns);

                current_key_start = rows_aggregated;
                rows_with_current_key = 0;
            }

            if (ttl_expired)
            {
                ++rows_with_current_key;
                ++rows_aggregated;
                for (const auto & name : column_names)
                {
                    const IColumn * values_column = block.getByName(name).column.get();
                    auto & column = aggregate_columns[header.getPositionByName(name)];
                    column->insertFrom(*values_column, i);
                }

                /// Track first/last row values for columns that need clamping
                if (!sorting_key_columns_to_clamp.empty())
                {
                    for (const auto & col_name : sorting_key_columns_to_clamp)
                    {
                        const IColumn * values_column = block.getByName(col_name).column.get();
                        if (!has_first_row)
                            values_column->get(i, first_row_values[col_name]);
                        values_column->get(i, last_row_values[col_name]);
                    }
                    has_first_row = true;
                }
            }
            else
            {
                for (const auto & name : column_names)
                {
                    const IColumn * values_column = block.getByName(name).column.get();
                    auto & column = result_columns[header.getPositionByName(name)];
                    column->insertFrom(*values_column, i);
                }
            }
        }

        if (rows_with_current_key)
        {
            some_rows_were_aggregated = true;
            calculateAggregates(aggregate_columns, current_key_start, rows_with_current_key);
        }
    }

    block = header.cloneWithColumns(std::move(result_columns));

    /// Verify sort order is maintained after TTL aggregation + SET.
    /// If SET modified columns used by the sorting key expression,
    /// the clamping above should have prevented violations, but we check anyway.
    if (sorting_key_expression && block.rows() > 0)
    {
        Block sort_key_block = block.cloneWithColumns(block.getColumns());
        sorting_key_expression->execute(sort_key_block);

        /// Check consecutive rows within this block
        for (size_t i = 1; i < sort_key_block.rows(); ++i)
        {
            for (const auto & sort_col : sort_description)
            {
                const IColumn & col = *sort_key_block.getByName(sort_col.column_name).column;
                int cmp = col.compareAt(i - 1, i, col, sort_col.nulls_direction) * sort_col.direction;
                if (cmp > 0)
                {
                    throw Exception(ErrorCodes::BAD_TTL_EXPRESSION,
                        "Sort order violation after TTL GROUP BY aggregation: "
                        "the SET clause modifies columns used in the sorting key expression. "
                        "Consider removing or adjusting the SET clause in the TTL expression "
                        "to avoid changing columns that affect the ORDER BY key");
                }
                if (cmp < 0)
                    break;
            }
        }

        /// Check continuity with the previous block
        if (!last_block_sort_key_columns.empty())
        {
            for (size_t j = 0; j < sort_description.size(); ++j)
            {
                const auto & sort_col = sort_description[j];
                const IColumn & cur_col = *sort_key_block.getByName(sort_col.column_name).column;
                int cmp = last_block_sort_key_columns[j]->compareAt(0, 0, cur_col, sort_col.nulls_direction) * sort_col.direction;
                if (cmp > 0)
                {
                    throw Exception(ErrorCodes::BAD_TTL_EXPRESSION,
                        "Sort order violation after TTL GROUP BY aggregation: "
                        "the SET clause modifies columns used in the sorting key expression. "
                        "Consider removing or adjusting the SET clause in the TTL expression "
                        "to avoid changing columns that affect the ORDER BY key");
                }
                if (cmp < 0)
                    break;
            }
        }

        /// Save last row for cross-block comparison (stored in sort_description order)
        size_t last_row = sort_key_block.rows() - 1;
        last_block_sort_key_columns.clear();
        for (const auto & sort_col : sort_description)
        {
            const auto & col = sort_key_block.getByName(sort_col.column_name).column;
            last_block_sort_key_columns.push_back(col->cut(last_row, 1));
        }
    }

    /// If some rows were aggregated we have to recalculate ttl info's
    if (some_rows_were_aggregated)
    {
        auto ttl_column_after_aggregation = executeExpressionAndGetColumn(ttl_expressions.expression, block, description.result_column);
        auto where_column_after_aggregation = executeExpressionAndGetColumn(ttl_expressions.where_expression, block, description.where_result_column);
        for (size_t i = 0; i < block.rows(); ++i)
        {
            bool where_filter_passed = !where_column_after_aggregation || where_column_after_aggregation->getBool(i);
            if (where_filter_passed)
                new_ttl_info.update(getTimestampByIndex(ttl_column_after_aggregation.get(), i));
        }
    }
}

void TTLAggregationAlgorithm::calculateAggregates(const MutableColumns & aggregate_columns, size_t start_pos, size_t length)
{
    Columns aggregate_chunk;
    aggregate_chunk.reserve(aggregate_columns.size());
    for (const auto & name : header.getNames())
    {
        const auto & column = aggregate_columns[header.getPositionByName(name)];
        ColumnPtr chunk_column = column->cut(start_pos, length);
        aggregate_chunk.emplace_back(std::move(chunk_column));
    }

    aggregator->executeOnBlock(
        aggregate_chunk, /* row_begin= */ 0, length,
        aggregation_result, key_columns, columns_for_aggregator, no_more_keys);

}

void TTLAggregationAlgorithm::finalizeAggregates(MutableColumns & result_columns)
{
    if (!aggregation_result.empty())
    {
        auto aggregated_res = aggregator->convertToBlocks(aggregation_result, true);

        for (auto & agg_block : aggregated_res)
        {
            for (const auto & it : description.set_parts)
            {
                it.expression->execute(agg_block);

                /// Restore LowCardinality wrappers on SET expression results if needed
                /// Aggregation strips LowCardinality, but result_columns expects it
                const auto & result_column_type = header.getByName(it.column_name).type;
                if (result_column_type->lowCardinality())
                {
                    auto & column_with_type = agg_block.getByName(it.expression_result_column_name);
                    // Only convert if the column doesn't already have LowCardinality
                    if (!column_with_type.type->lowCardinality())
                    {
                        auto nested_type = recursiveRemoveLowCardinality(result_column_type);
                        column_with_type.column = recursiveLowCardinalityTypeConversion(
                            column_with_type.column, nested_type, result_column_type);
                        column_with_type.type = result_column_type;
                    }
                }
            }

            /// Clamp SET-modified columns if they affect the sorting key.
            /// After aggregation + SET, the result row's sorting key must stay within
            /// [first_row_key, last_row_key] of the original group. If it falls outside,
            /// replace the SET-modified source columns with first/last row's original values.
            if (!sorting_key_columns_to_clamp.empty() && has_first_row)
            {
                for (size_t row = 0; row < agg_block.rows(); ++row)
                {
                    /// Build a temporary block with the result's source columns to compute sorting key
                    auto build_sort_key_block = [&](const std::function<Field(const String &)> & get_value) -> Block
                    {
                        Block tmp;
                        for (const auto & required_col : sorting_key_expression->getRequiredColumnsWithTypes())
                        {
                            auto col = required_col.type->createColumn();
                            col->insert(get_value(required_col.name));
                            tmp.insert({std::move(col), required_col.type, required_col.name});
                        }
                        sorting_key_expression->execute(tmp);
                        return tmp;
                    };

                    /// Get current result values for the required columns
                    auto get_result_value = [&](const String & col_name) -> Field
                    {
                        /// For columns modified by SET, use the SET result
                        for (const auto & it : description.set_parts)
                        {
                            if (it.column_name == col_name)
                            {
                                Field val;
                                agg_block.getByName(it.expression_result_column_name).column->get(row, val);
                                return val;
                            }
                        }
                        /// For GROUP BY key columns, use the aggregated key
                        if (agg_block.has(col_name))
                        {
                            Field val;
                            agg_block.getByName(col_name).column->get(row, val);
                            return val;
                        }
                        /// Fallback to first_row_values if available
                        if (auto it = first_row_values.find(col_name); it != first_row_values.end())
                            return it->second;
                        return Field();
                    };

                    Block result_key_block = build_sort_key_block(get_result_value);

                    Block first_key_block = build_sort_key_block([&](const String & col_name) -> Field
                    {
                        if (auto it = first_row_values.find(col_name); it != first_row_values.end())
                            return it->second;
                        return get_result_value(col_name);
                    });

                    Block last_key_block = build_sort_key_block([&](const String & col_name) -> Field
                    {
                        if (auto it = last_row_values.find(col_name); it != last_row_values.end())
                            return it->second;
                        return get_result_value(col_name);
                    });

                    /// Compare result sorting key with min (first row) and max (last row)
                    auto compare_sort_keys = [&](const Block & lhs, const Block & rhs) -> int
                    {
                        for (const auto & sort_col : sort_description)
                        {
                            const IColumn & lhs_col = *lhs.getByName(sort_col.column_name).column;
                            const IColumn & rhs_col = *rhs.getByName(sort_col.column_name).column;
                            int cmp = lhs_col.compareAt(0, 0, rhs_col, sort_col.nulls_direction) * sort_col.direction;
                            if (cmp != 0)
                                return cmp;
                        }
                        return 0;
                    };

                    bool need_clamp = false;
                    const std::unordered_map<String, Field> * clamp_source = nullptr;

                    if (compare_sort_keys(result_key_block, first_key_block) < 0)
                    {
                        need_clamp = true;
                        clamp_source = &first_row_values;
                    }
                    else if (compare_sort_keys(result_key_block, last_key_block) > 0)
                    {
                        need_clamp = true;
                        clamp_source = &last_row_values;
                    }

                    if (need_clamp)
                    {
                        for (const auto & col_name : sorting_key_columns_to_clamp)
                        {
                            /// Find the SET part for this column and replace its value in agg_block
                            for (const auto & it : description.set_parts)
                            {
                                if (it.column_name == col_name)
                                {
                                    auto & col_with_type = agg_block.getByName(it.expression_result_column_name);
                                    auto mutable_col = col_with_type.column->assumeMutable();
                                    auto replacement = col_with_type.type->createColumn();
                                    for (size_t r = 0; r < agg_block.rows(); ++r)
                                    {
                                        if (r == row)
                                            replacement->insert(clamp_source->at(col_name));
                                        else
                                        {
                                            Field val;
                                            mutable_col->get(r, val);
                                            replacement->insert(val);
                                        }
                                    }
                                    col_with_type.column = std::move(replacement);
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            /// Since there might be intersecting columns between GROUP BY and SET, we prioritize
            /// the SET values over the GROUP BY because doing it the other way causes unexpected
            /// results.
            std::unordered_set<String> columns_added;
            for (const auto & it : description.set_parts)
            {
                const IColumn * values_column = agg_block.getByName(it.expression_result_column_name).column.get();
                auto & result_column = result_columns[header.getPositionByName(it.column_name)];
                result_column->insertRangeFrom(*values_column, 0, agg_block.rows());
                columns_added.emplace(it.column_name);
            }

            for (const auto & name : description.group_by_keys)
            {
                if (!columns_added.contains(name))
                {
                    const IColumn * values_column = agg_block.getByName(name).column.get();
                    auto & result_column = result_columns[header.getPositionByName(name)];
                    result_column->insertRangeFrom(*values_column, 0, agg_block.rows());
                }
            }
        }
    }

    /// Reset first/last row tracking for the next group
    has_first_row = false;
    first_row_values.clear();
    last_row_values.clear();

    aggregation_result.invalidate();
}

void TTLAggregationAlgorithm::finalize(const MutableDataPartPtr & data_part) const
{
    if (new_ttl_info.finished())
    {
        data_part->ttl_infos.group_by_ttl[description.result_column] = new_ttl_info;
        data_part->ttl_infos.updatePartMinMaxTTL(new_ttl_info.min, new_ttl_info.max);
        return;
    }
    data_part->ttl_infos.group_by_ttl[description.result_column] = old_ttl_info;
    data_part->ttl_infos.updatePartMinMaxTTL(old_ttl_info.min, old_ttl_info.max);
}

}
