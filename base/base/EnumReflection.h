#pragma once

#include <fmt/format.h>
#include <type_traits>
#include <string_view>

template <typename T> concept is_enum = std::is_enum_v<T>;

template <typename T>
concept has_enumToString = is_enum<T> && requires(T value) { { enumToString(value) } -> std::convertible_to<std::string_view>; };

/// Generic fmt formatter for enums - requires enumToString(EnumType) via ADL
template <has_enumToString T>
struct fmt::formatter<T> : fmt::formatter<std::string_view>
{
    constexpr auto format(T value, auto & format_context) const
    {
        return formatter<string_view>::format(enumToString(value), format_context);
    }
};
