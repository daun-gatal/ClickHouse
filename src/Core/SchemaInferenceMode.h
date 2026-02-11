#pragma once

#include <cstdint>
#include <stdexcept>
#include <string_view>

namespace DB
{

enum class SchemaInferenceMode : uint8_t
{
    DEFAULT,
    UNION,
};

inline std::string_view enumToString(SchemaInferenceMode mode)
{
    switch (mode)
    {
        case SchemaInferenceMode::DEFAULT: return "DEFAULT";
        case SchemaInferenceMode::UNION: return "UNION";
    }
    throw std::logic_error("Unknown SchemaInferenceMode");
}

}
