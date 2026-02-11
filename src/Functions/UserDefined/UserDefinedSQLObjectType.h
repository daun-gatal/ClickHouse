#pragma once

#include <cstdint>
#include <string_view>

namespace DB
{

enum class UserDefinedSQLObjectType : uint8_t
{
    Function
};

inline std::string_view enumToString(UserDefinedSQLObjectType value)
{
    switch (value)
    {
        case UserDefinedSQLObjectType::Function: return "Function";
    }
    return "Unknown";
}

}
