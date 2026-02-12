#include <Storages/MergeTree/ColumnsCache.h>

namespace ProfileEvents
{
    extern const Event ColumnsCacheHits;
    extern const Event ColumnsCacheMisses;
    extern const Event ColumnsCacheEvictedBytes;
    extern const Event ColumnsCacheEvictedEntries;
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

    ColumnIdentifier id{table_uuid, part_name, column_name};
    auto it = interval_index.find(id);
    if (it == interval_index.end())
        return result;

    const auto & intervals = it->second;

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

}
