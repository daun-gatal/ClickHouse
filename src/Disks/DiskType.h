#pragma once

#include <base/defines.h>
#include <base/types.h>
#include <string_view>

namespace DB
{

enum class DataSourceType : uint8_t
{
    Local,
    RAM,
    ObjectStorage,
};

enum class ObjectStorageType : uint8_t
{
    None = 0,
    S3 = 1,
    Azure = 2,
    HDFS = 3,
    Web = 4,
    Local = 5,

    Max = 6,
};

enum class MetadataStorageType : uint8_t
{
    None,
    Local,
    Keeper,
    Plain,
    PlainRewritable,
    StaticWeb,
    Memory,
};

MetadataStorageType metadataTypeFromString(const String & type);

inline std::string_view toString(DataSourceType type)
{
    switch (type)
    {
        case DataSourceType::Local: return "Local";
        case DataSourceType::RAM: return "RAM";
        case DataSourceType::ObjectStorage: return "ObjectStorage";
    }
}

inline std::string_view toString(ObjectStorageType type)
{
    switch (type)
    {
        case ObjectStorageType::None: return "None";
        case ObjectStorageType::S3: return "S3";
        case ObjectStorageType::Azure: return "Azure";
        case ObjectStorageType::HDFS: return "HDFS";
        case ObjectStorageType::Web: return "Web";
        case ObjectStorageType::Local: return "Local";
        case ObjectStorageType::Max: return "Max";
    }
}

inline std::string_view toString(MetadataStorageType type)
{
    switch (type)
    {
        case MetadataStorageType::None: return "None";
        case MetadataStorageType::Local: return "Local";
        case MetadataStorageType::Keeper: return "Keeper";
        case MetadataStorageType::Plain: return "Plain";
        case MetadataStorageType::PlainRewritable: return "PlainRewritable";
        case MetadataStorageType::StaticWeb: return "StaticWeb";
        case MetadataStorageType::Memory: return "Memory";
    }
}

struct DataSourceDescription
{
    DataSourceType type;
    ObjectStorageType object_storage_type = ObjectStorageType::None;
    MetadataStorageType metadata_type = MetadataStorageType::None;

    String description;

    bool is_encrypted = false;
    bool is_cached = false;

    String zookeeper_name;

    bool operator==(const DataSourceDescription & other) const;
    bool sameKind(const DataSourceDescription & other) const;

    String name() const;

    /// Returns a string with the name and all the fields of the DataSourceDescription
    String toString() const;
};

}
