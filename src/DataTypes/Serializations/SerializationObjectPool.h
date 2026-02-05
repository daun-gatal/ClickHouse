#pragma once

#include <DataTypes/Serializations/ISerialization.h>

#include <shared_mutex>
#include <unordered_map>

namespace DB
{

/// Pool for constant serialization objects.
/// Used to create them only once and share them between different columns.
class SerializationObjectPool
{
public:
    static SerializationObjectPool & instance()
    {
        static SerializationObjectPool cache;
        return cache;
    }

    SerializationPtr getOrCreate(const String & key, SerializationPtr serialization)
    {
        std::unique_lock lock(mutex);

        auto it = cache.find(key);
        if (it != cache.end())
            return it->second;

        cache[key] = serialization;
        return serialization;
    }

    void remove(const String & key)
    {
        std::unique_lock lock(mutex);
        auto it = cache.find(key);
        /// use_count == 2 means: one in cache, one held by the object being destroyed
        if (it != cache.end() && it->second.use_count() == 2)
            cache.erase(it);
    }

    bool contains(const String & key) const
    {
        std::shared_lock lock(mutex);
        return cache.contains(key);
    }

    size_t size() const
    {
        std::shared_lock lock(mutex);
        return cache.size();
    }

    void clear()
    {
        std::unique_lock lock(mutex);
        cache.clear();
    }

private:
    SerializationObjectPool() = default;

    mutable std::shared_mutex mutex;
    std::unordered_map<String, SerializationPtr> cache;
};

}
