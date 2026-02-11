#pragma once

#include <string_view>
#include <stdexcept>

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

inline std::string_view toString(DataDestinationType type)
{
    switch (type)
    {
        case DataDestinationType::DISK:   return "DISK";
        case DataDestinationType::VOLUME: return "VOLUME";
        case DataDestinationType::TABLE:  return "TABLE";
        case DataDestinationType::DELETE: return "DELETE";
        case DataDestinationType::SHARD:  return "SHARD";
    }
    throw std::logic_error("Unknown DataDestinationType");
}

}
