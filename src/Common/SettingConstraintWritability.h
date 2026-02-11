#pragma once

#include <cstdint>
#include <string_view>

namespace DB
{

enum class SettingConstraintWritability : uint8_t
{
    // Default. Setting can be change within specified range only in `readonly=0` or `readonly=2` mode.
    WRITABLE,

    // Setting cannot be changed at all.
    // Either READONLY or CONST keyword in SQL syntax can be used (<readonly/> or <const/> in config.xml) to enable this.
    // NOTE: name `CONST` is chosen to avoid confusion with `readonly` setting.
    CONST,

    // Setting can be changed within specified range, regardless of `readonly` setting value.
    CHANGEABLE_IN_READONLY,

    MAX
};

inline std::string_view enumToString(SettingConstraintWritability value)
{
    switch (value)
    {
        case SettingConstraintWritability::WRITABLE: return "WRITABLE";
        case SettingConstraintWritability::CONST: return "CONST";
        case SettingConstraintWritability::CHANGEABLE_IN_READONLY: return "CHANGEABLE_IN_READONLY";
        case SettingConstraintWritability::MAX: return "MAX";
    }
    return "Unknown";
}

}
