#pragma once
#include <cstdint>
#include <string_view>
#include <stdexcept>

namespace DB
{
enum class SyncReplicaMode : uint8_t
{
    DEFAULT,
    STRICT,
    LIGHTWEIGHT,
    PULL,
};

inline std::string_view enumToString(SyncReplicaMode mode)
{
    switch (mode)
    {
        case SyncReplicaMode::DEFAULT:     return "DEFAULT";
        case SyncReplicaMode::STRICT:      return "STRICT";
        case SyncReplicaMode::LIGHTWEIGHT: return "LIGHTWEIGHT";
        case SyncReplicaMode::PULL:        return "PULL";
    }
    throw std::runtime_error("Unknown SyncReplicaMode");
}
}
