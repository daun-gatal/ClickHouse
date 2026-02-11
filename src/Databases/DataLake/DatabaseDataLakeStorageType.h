#pragma once
#include <Core/Types.h>

#include <optional>
#include <string_view>
#include <stdexcept>

namespace DB
{

enum class DatabaseDataLakeStorageType : uint8_t
{
    S3,
    Azure,
    Local,
    HDFS,
    /// Fake storage in case when catalog store not only
    /// primary-type tables (DeltaLake or Iceberg), but, for
    /// example, something else like INFORMATION_SCHEMA.
    /// Such tables are unreadable, but at least we can show
    /// them in SHOW CREATE TABLE, as well we can show their
    /// schema.
    Other,
};

inline std::string_view enumToString(DatabaseDataLakeStorageType type)
{
    switch (type)
    {
        case DatabaseDataLakeStorageType::S3: return "S3";
        case DatabaseDataLakeStorageType::Azure: return "Azure";
        case DatabaseDataLakeStorageType::Local: return "Local";
        case DatabaseDataLakeStorageType::HDFS: return "HDFS";
        case DatabaseDataLakeStorageType::Other: return "Other";
    }
    throw std::logic_error("Unknown DatabaseDataLakeStorageType");
}

/// Case-sensitive conversion from string to DatabaseDataLakeStorageType.
inline std::optional<DatabaseDataLakeStorageType> databaseDataLakeStorageTypeFromString(const std::string & name)
{
    if (name == "S3") return DatabaseDataLakeStorageType::S3;
    if (name == "Azure") return DatabaseDataLakeStorageType::Azure;
    if (name == "Local") return DatabaseDataLakeStorageType::Local;
    if (name == "HDFS") return DatabaseDataLakeStorageType::HDFS;
    if (name == "Other") return DatabaseDataLakeStorageType::Other;
    return std::nullopt;
}

}
