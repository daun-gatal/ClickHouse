#include <DataTypes/Serializations/SerializationObjectPool.h>
#include <DataTypes/Serializations/ISerialization.h>

#include <gtest/gtest.h>

namespace DB
{

/// Mock serialization class for testing
class MockSerialization : public ISerialization
{
public:
    explicit MockSerialization(String name_) : name(std::move(name_)) {}

    ~MockSerialization() noexcept(false) override
    {
        SerializationObjectPool::instance().remove(getName());
    }

    String getName() const override { return name; }

    void serializeBinary(const Field &, WriteBuffer &, const FormatSettings &) const override {}
    void deserializeBinary(Field &, ReadBuffer &, const FormatSettings &) const override {}
    void serializeBinary(const IColumn &, size_t, WriteBuffer &, const FormatSettings &) const override {}
    void deserializeBinary(IColumn &, ReadBuffer &, const FormatSettings &) const override {}
    void serializeText(const IColumn &, size_t, WriteBuffer &, const FormatSettings &) const override {}
    void deserializeWholeText(IColumn &, ReadBuffer &, const FormatSettings &) const override {}
    void serializeTextEscaped(const IColumn &, size_t, WriteBuffer &, const FormatSettings &) const override {}
    void deserializeTextEscaped(IColumn &, ReadBuffer &, const FormatSettings &) const override {}
    void serializeTextQuoted(const IColumn &, size_t, WriteBuffer &, const FormatSettings &) const override {}
    void deserializeTextQuoted(IColumn &, ReadBuffer &, const FormatSettings &) const override {}
    void serializeTextJSON(const IColumn &, size_t, WriteBuffer &, const FormatSettings &) const override {}
    void deserializeTextJSON(IColumn &, ReadBuffer &, const FormatSettings &) const override {}
    void serializeTextCSV(const IColumn &, size_t, WriteBuffer &, const FormatSettings &) const override {}
    void deserializeTextCSV(IColumn &, ReadBuffer &, const FormatSettings &) const override {}

private:
    String name;
};

TEST(SerializationObjectPool, RepeatedGetOrCreateReturnsSamePointer)
{
    auto & pool = SerializationObjectPool::instance();
    const String key = "test_key_repeated";

    // First call: insert a new serialization
    auto serialization1 = std::make_shared<MockSerialization>(key);
    auto ptr1 = serialization1.get();
    auto result1 = pool.getOrCreate(key, std::move(serialization1));

    // Second call: should return the cached instance, not create a new one
    auto serialization2 = std::make_shared<MockSerialization>(key);
    auto ptr2 = serialization2.get();
    auto result2 = pool.getOrCreate(key, std::move(serialization2));

    // Both results should point to the same object (the first one that was inserted)
    EXPECT_EQ(result1.get(), result2.get());
    EXPECT_EQ(result1.get(), ptr1);
    EXPECT_NE(result2.get(), ptr2);  // The second serialization should not have been used

    // Third call: verify it still returns the same cached instance
    auto serialization3 = std::make_shared<MockSerialization>(key);
    auto result3 = pool.getOrCreate(key, std::move(serialization3));
    EXPECT_EQ(result1.get(), result3.get());
}

TEST(SerializationObjectPool, DistinctKeysYieldDistinctInstances)
{
    auto & pool = SerializationObjectPool::instance();
    const String key1 = "test_key_distinct_1";
    const String key2 = "test_key_distinct_2";
    const String key3 = "test_key_distinct_3";

    // Create three serializations with different keys
    auto serialization1 = std::make_shared<MockSerialization>(key1);
    auto ptr1 = serialization1.get();
    auto result1 = pool.getOrCreate(key1, std::move(serialization1));

    auto serialization2 = std::make_shared<MockSerialization>(key2);
    auto ptr2 = serialization2.get();
    auto result2 = pool.getOrCreate(key2, std::move(serialization2));

    auto serialization3 = std::make_shared<MockSerialization>(key3);
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

    // Create and cache a serialization
    auto serialization1 = std::make_shared<MockSerialization>(key);
    auto ptr1 = serialization1.get();
    auto result1 = pool.getOrCreate(key, std::move(serialization1));

    // Keep a reference to the first instance
    SerializationPtr held_reference = result1;

    // Try to insert a new serialization with the same key multiple times
    for (int i = 0; i < 5; ++i)
    {
        auto serialization = std::make_shared<MockSerialization>(key);
        auto result = pool.getOrCreate(key, std::move(serialization));
        
        // Should always return the original cached instance
        EXPECT_EQ(result.get(), ptr1);
        EXPECT_EQ(result.get(), held_reference.get());
    }

    // Even after multiple attempts, the original instance should still be in the cache
    auto final_check = pool.getOrCreate(key, std::make_shared<MockSerialization>(key));
    EXPECT_EQ(final_check.get(), ptr1);
}

}
