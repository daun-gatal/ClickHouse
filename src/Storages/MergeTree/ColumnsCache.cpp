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

}
