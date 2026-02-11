#pragma once

#include <string_view>

namespace DB
{

/// Mode of opening a file for write.
enum class WriteMode : uint8_t
{
    Rewrite,
    Append
};

constexpr std::string_view enumToString(WriteMode mode)
{
    switch (mode)
    {
        case WriteMode::Rewrite: return "Rewrite";
        case WriteMode::Append: return "Append";
    }
    return "Unknown";
}

}
