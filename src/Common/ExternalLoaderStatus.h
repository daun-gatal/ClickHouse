#pragma once

#include <vector>
#include <string_view>
#include <base/types.h>

namespace DB
{
enum class ExternalLoaderStatus : int8_t
{
    NOT_LOADED, /// Object hasn't been tried to load. This is an initial state.
    LOADED, /// Object has been loaded successfully.
    FAILED, /// Object has been failed to load.
    LOADING, /// Object is being loaded right now for the first time.
    FAILED_AND_RELOADING, /// Object was failed to load before and it's being reloaded right now.
    LOADED_AND_RELOADING, /// Object was loaded successfully before and it's being reloaded right now.
    NOT_EXIST, /// Object with this name wasn't found in the configuration.
};

inline constexpr size_t ExternalLoaderStatusCount = 7;

inline std::string_view enumToString(ExternalLoaderStatus status)
{
    switch (status)
    {
        case ExternalLoaderStatus::NOT_LOADED: return "NOT_LOADED";
        case ExternalLoaderStatus::LOADED: return "LOADED";
        case ExternalLoaderStatus::FAILED: return "FAILED";
        case ExternalLoaderStatus::LOADING: return "LOADING";
        case ExternalLoaderStatus::FAILED_AND_RELOADING: return "FAILED_AND_RELOADING";
        case ExternalLoaderStatus::LOADED_AND_RELOADING: return "LOADED_AND_RELOADING";
        case ExternalLoaderStatus::NOT_EXIST: return "NOT_EXIST";
    }
}

std::vector<std::pair<String, Int8>> getExternalLoaderStatusEnumAllPossibleValues();
}
