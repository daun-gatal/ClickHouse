#pragma once

#include <concepts>
#include <cstdint>
#include <fmt/format.h>
#include <type_traits>
#include <string_view>

template <typename T> concept is_enum = std::is_enum_v<T>;

/// Enable printing enum values as strings via fmt.
/// If toString(EnumType) is available via ADL, it is used.
/// Otherwise, falls back to printing the integer value.
template <is_enum T>
struct fmt::formatter<T>
{
    constexpr auto parse(auto & ctx) { return ctx.begin(); }

    auto format(T value, auto & ctx) const
    {
        if constexpr (requires { { toString(value) } -> std::convertible_to<std::string_view>; })
            return fmt::format_to(ctx.out(), "{}", toString(value));
        else
            return fmt::format_to(ctx.out(), "{}", static_cast<int64_t>(static_cast<std::underlying_type_t<T>>(value)));
    }
};
