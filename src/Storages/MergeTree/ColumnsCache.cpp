#include <Storages/MergeTree/ColumnsCache.h>

namespace ProfileEvents
{
    extern const Event ColumnsCacheHits;
    extern const Event ColumnsCacheMisses;
}

namespace DB
{

template class CacheBase<ColumnsCacheKey, ColumnsCacheEntry, ColumnsCacheKeyHash, ColumnsCacheWeightFunction>;

ColumnsCache::ColumnsCache(
    const String & cache_policy,
    CurrentMetrics::Metric size_in_bytes_metric,
    CurrentMetrics::Metric count_metric,
    size_t max_size_in_bytes,
    size_t max_count,
    double size_ratio)
    : Base(cache_policy, size_in_bytes_metric, count_metric, max_size_in_bytes, max_count, size_ratio)
{
}

std::vector<std::pair<ColumnsCache::Key, ColumnsCache::MappedPtr>>
ColumnsCache::getIntersecting(
    const UUID & table_uuid,
    const String & part_name,
    const String & column_name,
    size_t row_begin,
    size_t row_end)
{
    std::vector<std::pair<Key, MappedPtr>> result;

    std::lock_guard lock(interval_index_mutex);

    PartIdentifier part_id{table_uuid, part_name};
    auto part_it = interval_index.find(part_id);
    if (part_it == interval_index.end())
        return result;

    const auto & columns_map = part_it->second;
    auto column_it = columns_map.find(column_name);
    if (column_it == columns_map.end())
        return result;

    const auto & intervals = column_it->second;

    /// Find the first interval that could potentially intersect
    /// (last interval with row_begin <= row_end)
    auto start_it = intervals.upper_bound(row_end);
    if (start_it != intervals.begin())
        --start_it;

    /// Collect all intersecting intervals
    for (auto interval_it = start_it; interval_it != intervals.end(); ++interval_it)
    {
        const auto & key = interval_it->second;

        /// Stop if we've gone past the query range
        if (key.row_begin >= row_end)
            break;

        /// Check if this interval actually intersects
        if (key.row_end > row_begin)
        {
            /// Verify the entry still exists in cache (might have been evicted)
            auto entry = Base::get(key);
            if (entry)
            {
                result.emplace_back(key, entry);
                ProfileEvents::increment(ProfileEvents::ColumnsCacheHits);
            }
            else
            {
                /// Entry was evicted, we could remove it from interval_index here,
                /// but it's not critical - will be cleaned up on next set()
                ProfileEvents::increment(ProfileEvents::ColumnsCacheMisses);
            }
        }
    }

    return result;
}

void ColumnsCache::removePart(const UUID & table_uuid, const String & part_name)
{
    std::lock_guard lock(interval_index_mutex);

    PartIdentifier part_id{table_uuid, part_name};
    auto part_it = interval_index.find(part_id);
    if (part_it == interval_index.end())
        return;

    /// Remove all cache entries for this part
    const auto & columns_map = part_it->second;
    for (const auto & [column_name, intervals] : columns_map)
    {
        for (const auto & [row_begin, key] : intervals)
        {
            /// Remove from the underlying cache
            Base::remove(key);
        }
    }

    /// Remove the part from the interval index
    interval_index.erase(part_it);
}

std::vector<std::pair<ColumnsCache::Key, ColumnsCache::MappedPtr>>
ColumnsCache::getAllEntries()
{
    std::vector<std::pair<Key, MappedPtr>> result;

    std::lock_guard lock(interval_index_mutex);

    /// Iterate through all parts, columns, and intervals
    for (const auto & [part_id, columns_map] : interval_index)
    {
        for (const auto & [column_name, intervals] : columns_map)
        {
            for (const auto & [row_begin, key] : intervals)
            {
                /// Verify the entry still exists in cache (might have been evicted)
                auto entry = Base::get(key);
                if (entry)
                {
                    result.emplace_back(key, entry);
                }
            }
        }
    }

    return result;
}

}
