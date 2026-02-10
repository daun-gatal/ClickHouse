#pragma once

#include <DataTypes/Serializations/ISerialization.h>

#include <mutex>
#include <unordered_map>

namespace DB
{

/// Pool for constant serialization objects.
/// Deduplicates identical serializations so that concurrent users of the
/// same type share one object. Uses weak_ptr so entries expire automatically
/// when all external holders release their references — no remove() or
/// destructor interaction needed.
class SerializationObjectPool
{
public:
    static SerializationObjectPool & instance()
    {
        static SerializationObjectPool pool;
        return pool;
    }

    SerializationPtr getOrCreate(const String & key, SerializationPtr && serialization)
    {
        std::lock_guard lock(mutex);

        auto it = cache.find(key);
        if (it != cache.end())
        {
            if (auto existing = it->second.lock())
                return existing;
            /// Expired — replace with the new one.
            it->second = serialization;
        }
        else
        {
            cache.emplace(key, serialization);
        }

        /// Lazy cleanup: sweep expired entries when the map gets large.
        if (cache.size() > cleanup_threshold)
            removeExpiredEntries();

        return std::move(serialization);
    }

    /// No-op kept for backward compatibility with existing destructors.
    /// With weak_ptr storage, entries expire on their own.
    void remove(const String & /*key*/) {}

private:
    SerializationObjectPool() = default;

    void removeExpiredEntries()
    {
        std::erase_if(cache, [](const auto & pair) { return pair.second.expired(); });
    }

    static constexpr size_t cleanup_threshold = 1000;

    /// Recursive mutex: SerializationLowCardinality's constructor creates
    /// inner serializations that also go through the pool.
    mutable std::recursive_mutex mutex;
    std::unordered_map<String, std::weak_ptr<const ISerialization>> cache;
};

}
