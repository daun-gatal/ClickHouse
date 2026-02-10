#include <DataTypes/Serializations/SerializationObjectPool.h>
#include <DataTypes/Serializations/SerializationNothing.h>
#include <DataTypes/Serializations/SerializationWrapper.h>

#include <gtest/gtest.h>

namespace DB
{

/// Test wrapper with custom name for testing object pool behavior
class TestSerializationWrapper : public SerializationWrapper
{
public:
    TestSerializationWrapper(const SerializationPtr & nested, String custom_name_)
        : SerializationWrapper(nested)
        , custom_name(std::move(custom_name_))
    {
    }

    ~TestSerializationWrapper() noexcept(false) override
    {
        SerializationObjectPool::instance().remove(getName());
    }

    String getName() const override { return custom_name; }

private:
    String custom_name;
};

TEST(SerializationObjectPool, RepeatedGetOrCreateReturnsSamePointer)
{
    auto & pool = SerializationObjectPool::instance();
    const String key = "test_key_repeated";

    // Create a base serialization to wrap (SerializationNothing is simple and doesn't use the pool itself)
    auto base = SerializationNothing::create();

    // First call: insert a new serialization
    auto serialization1 = std::make_shared<TestSerializationWrapper>(base, key);
    auto ptr1 = serialization1.get();
    auto result1 = pool.getOrCreate(key, std::move(serialization1));

    // Second call: should return the cached instance, not create a new one
    auto serialization2 = std::make_shared<TestSerializationWrapper>(base, key);
    auto ptr2 = serialization2.get();
    auto result2 = pool.getOrCreate(key, std::move(serialization2));

    // Both results should point to the same object (the first one that was inserted)
    EXPECT_EQ(result1.get(), result2.get());
    EXPECT_EQ(result1.get(), ptr1);
    EXPECT_NE(result2.get(), ptr2);  // The second serialization should not have been used

    // Third call: verify it still returns the same cached instance
    auto serialization3 = std::make_shared<TestSerializationWrapper>(base, key);
    auto result3 = pool.getOrCreate(key, std::move(serialization3));
    EXPECT_EQ(result1.get(), result3.get());
}

TEST(SerializationObjectPool, DistinctKeysYieldDistinctInstances)
{
    auto & pool = SerializationObjectPool::instance();
    const String key1 = "test_key_distinct_1";
    const String key2 = "test_key_distinct_2";
    const String key3 = "test_key_distinct_3";

    // Create a base serialization to wrap
    auto base = SerializationNothing::create();

    // Create three serializations with different keys
    auto serialization1 = std::make_shared<TestSerializationWrapper>(base, key1);
    auto ptr1 = serialization1.get();
    auto result1 = pool.getOrCreate(key1, std::move(serialization1));

    auto serialization2 = std::make_shared<TestSerializationWrapper>(base, key2);
    auto ptr2 = serialization2.get();
    auto result2 = pool.getOrCreate(key2, std::move(serialization2));

    auto serialization3 = std::make_shared<TestSerializationWrapper>(base, key3);
    auto ptr3 = serialization3.get();
    auto result3 = pool.getOrCreate(key3, std::move(serialization3));

    // All three should be distinct instances
    EXPECT_NE(result1.get(), result2.get());
    EXPECT_NE(result2.get(), result3.get());
    EXPECT_NE(result1.get(), result3.get());

    // Each result should match its original pointer
    EXPECT_EQ(result1.get(), ptr1);
    EXPECT_EQ(result2.get(), ptr2);
    EXPECT_EQ(result3.get(), ptr3);

    // Verify that getName() returns different values
    EXPECT_EQ(result1->getName(), key1);
    EXPECT_EQ(result2->getName(), key2);
    EXPECT_EQ(result3->getName(), key3);
}

TEST(SerializationObjectPool, CacheDoesNotEvictWhileInUse)
{
    auto & pool = SerializationObjectPool::instance();
    const String key = "test_key_no_evict";

    // Create a base serialization to wrap
    auto base = SerializationNothing::create();

    // Create and cache a serialization
    auto serialization1 = std::make_shared<TestSerializationWrapper>(base, key);
    auto ptr1 = serialization1.get();
    auto result1 = pool.getOrCreate(key, std::move(serialization1));

    // Keep a reference to the first instance
    SerializationPtr held_reference = result1;

    // Try to insert a new serialization with the same key multiple times
    for (int i = 0; i < 5; ++i)
    {
        (void)i; // Intentionally unused - we just need multiple iterations
        auto serialization = std::make_shared<TestSerializationWrapper>(base, key);
        auto result = pool.getOrCreate(key, std::move(serialization));
        
        // Should always return the original cached instance
        EXPECT_EQ(result.get(), ptr1);
        EXPECT_EQ(result.get(), held_reference.get());
    }

    // Even after multiple attempts, the original instance should still be in the cache
    auto final_check = pool.getOrCreate(key, std::make_shared<TestSerializationWrapper>(base, key));
    EXPECT_EQ(final_check.get(), ptr1);
}

}
