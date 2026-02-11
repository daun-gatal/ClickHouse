#pragma once

#include <string_view>

namespace DB
{

enum class DataDestinationType : uint8_t
{
    DISK,
    VOLUME,
    TABLE,
    DELETE,
    SHARD,
};

constexpr std::string_view enumToString(DataDestinationType type)
{
    switch (type)
    {
        case DataDestinationType::DISK: return "DISK";
        case DataDestinationType::VOLUME: return "VOLUME";
        case DataDestinationType::TABLE: return "TABLE";
        case DataDestinationType::DELETE: return "DELETE";
        case DataDestinationType::SHARD: return "SHARD";
    }
}

}
