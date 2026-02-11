#pragma once

#include <cstdint>
#include <string_view>

namespace DB
{

enum class AsynchronousInsertQueueDataKind : uint8_t
{
    Parsed = 0,
    Preprocessed = 1,
};

inline std::string_view enumToString(AsynchronousInsertQueueDataKind value)
{
    switch (value)
    {
        case AsynchronousInsertQueueDataKind::Parsed: return "Parsed";
        case AsynchronousInsertQueueDataKind::Preprocessed: return "Preprocessed";
    }
    return "Unknown";
}

}
