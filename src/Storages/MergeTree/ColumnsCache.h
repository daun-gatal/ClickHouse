#pragma once

#include <memory>

#include <Common/CacheBase.h>
#include <Common/ProfileEvents.h>
#include <Common/SipHash.h>
#include <Columns/IColumn.h>
#include <Core/UUID.h>
#include <Storages/MergeTree/MarkRange.h>



namespace ProfileEvents
{
    extern const Event ColumnsCacheHits;
    extern const Event ColumnsCacheMisses;
    extern const Event ColumnsCacheEvictedBytes;
    extern const Event ColumnsCacheEvictedEntries;
}

namespace DB
{

/// Key for looking up cached deserialized columns.
/// Identifies a specific column in a specific mark range of a specific data part.
/// Uses Table UUID so that RENAME TABLE properly invalidates the cache.
struct ColumnsCacheKey
{
    UUID table_uuid;
    String part_name;
    String column_name;
    size_t mark_begin;
    size_t mark_end;

    bool operator==(const ColumnsCacheKey & other) const = default;
};

struct ColumnsCacheKeyHash
{
    size_t operator()(const ColumnsCacheKey & key) const
    {
        SipHash hash;
        hash.update(key.table_uuid);
        hash.update(key.part_name);
        hash.update(key.column_name);
        hash.update(key.mark_begin);
        hash.update(key.mark_end);
        return hash.get64();
    }
};

/// Cached deserialized column data.
struct ColumnsCacheEntry
{
    ColumnPtr column;
    size_t rows;
};

struct ColumnsCacheWeightFunction
{
    /// Overhead for key storage, hash map entry, shared pointers, etc.
    static constexpr size_t COLUMNS_CACHE_OVERHEAD = 256;

    size_t operator()(const ColumnsCacheEntry & entry) const
    {
        return entry.column->byteSize() + COLUMNS_CACHE_OVERHEAD;
    }
};

extern template class CacheBase<ColumnsCacheKey, ColumnsCacheEntry, ColumnsCacheKeyHash, ColumnsCacheWeightFunction>;

/// Cache of deserialized columns for MergeTree tables.
/// Eliminates the need to read compressed data, decompress, and deserialize
/// for frequently accessed data parts and columns.
class ColumnsCache : public CacheBase<ColumnsCacheKey, ColumnsCacheEntry, ColumnsCacheKeyHash, ColumnsCacheWeightFunction>
{
private:
    using Base = CacheBase<ColumnsCacheKey, ColumnsCacheEntry, ColumnsCacheKeyHash, ColumnsCacheWeightFunction>;

public:
    ColumnsCache(
        const String & cache_policy,
        CurrentMetrics::Metric size_in_bytes_metric,
        CurrentMetrics::Metric count_metric,
        size_t max_size_in_bytes,
        size_t max_count,
        double size_ratio);

    /// Look up a cached column. Returns nullptr on miss.
    MappedPtr get(const Key & key)
    {
        auto result = Base::get(key);
        if (result)
            ProfileEvents::increment(ProfileEvents::ColumnsCacheHits);
        else
            ProfileEvents::increment(ProfileEvents::ColumnsCacheMisses);
        return result;
    }

    /// Insert a column into the cache.
    void set(const Key & key, const MappedPtr & mapped)
    {
        Base::set(key, mapped);
    }

private:
    void onEntryRemoval(size_t weight_loss, const MappedPtr &) override
    {
        ProfileEvents::increment(ProfileEvents::ColumnsCacheEvictedEntries);
        ProfileEvents::increment(ProfileEvents::ColumnsCacheEvictedBytes, weight_loss);
    }
};

using ColumnsCachePtr = std::shared_ptr<ColumnsCache>;

}
