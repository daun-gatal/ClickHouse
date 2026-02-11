#pragma once

#include <Coordination/KeeperConstants.h>
#include <Core/LogsLevel.h>

#include <cstddef>
#include <memory>
#include <string_view>
#include <optional>

namespace Poco
{
class Logger;
}

using LoggerPtr = std::shared_ptr<Poco::Logger>;

namespace DB
{

/// these values cannot be reordered or removed, only new values can be added
enum class KeeperFeatureFlag : size_t
{
    FILTERED_LIST = 0,
    MULTI_READ,
    CHECK_NOT_EXISTS,
    CREATE_IF_NOT_EXISTS,
    REMOVE_RECURSIVE,
    MULTI_WATCHES,
    CHECK_STAT,
    PERSISTENT_WATCHES,
    CREATE_WITH_STATS,
    TRY_REMOVE,
    LIST_WITH_STAT_AND_DATA,
};

inline constexpr KeeperFeatureFlag ALL_KEEPER_FEATURE_FLAGS[] = {
    KeeperFeatureFlag::FILTERED_LIST,
    KeeperFeatureFlag::MULTI_READ,
    KeeperFeatureFlag::CHECK_NOT_EXISTS,
    KeeperFeatureFlag::CREATE_IF_NOT_EXISTS,
    KeeperFeatureFlag::REMOVE_RECURSIVE,
    KeeperFeatureFlag::MULTI_WATCHES,
    KeeperFeatureFlag::CHECK_STAT,
    KeeperFeatureFlag::PERSISTENT_WATCHES,
    KeeperFeatureFlag::CREATE_WITH_STATS,
    KeeperFeatureFlag::TRY_REMOVE,
    KeeperFeatureFlag::LIST_WITH_STAT_AND_DATA,
};

inline constexpr size_t KEEPER_FEATURE_FLAG_COUNT = std::size(ALL_KEEPER_FEATURE_FLAGS);

constexpr std::string_view enumToString(KeeperFeatureFlag flag)
{
    switch (flag)
    {
        case KeeperFeatureFlag::FILTERED_LIST: return "FILTERED_LIST";
        case KeeperFeatureFlag::MULTI_READ: return "MULTI_READ";
        case KeeperFeatureFlag::CHECK_NOT_EXISTS: return "CHECK_NOT_EXISTS";
        case KeeperFeatureFlag::CREATE_IF_NOT_EXISTS: return "CREATE_IF_NOT_EXISTS";
        case KeeperFeatureFlag::REMOVE_RECURSIVE: return "REMOVE_RECURSIVE";
        case KeeperFeatureFlag::MULTI_WATCHES: return "MULTI_WATCHES";
        case KeeperFeatureFlag::CHECK_STAT: return "CHECK_STAT";
        case KeeperFeatureFlag::PERSISTENT_WATCHES: return "PERSISTENT_WATCHES";
        case KeeperFeatureFlag::CREATE_WITH_STATS: return "CREATE_WITH_STATS";
        case KeeperFeatureFlag::TRY_REMOVE: return "TRY_REMOVE";
        case KeeperFeatureFlag::LIST_WITH_STAT_AND_DATA: return "LIST_WITH_STAT_AND_DATA";
    }
    return "UNKNOWN";
}

/// Parse a KeeperFeatureFlag from its string name (case-sensitive).
/// Returns std::nullopt if the name does not match any known flag.
inline std::optional<KeeperFeatureFlag> keeperFeatureFlagFromString(std::string_view name)
{
    for (auto flag : ALL_KEEPER_FEATURE_FLAGS)
        if (enumToString(flag) == name)
            return flag;
    return std::nullopt;
}

class KeeperFeatureFlags
{
public:
    KeeperFeatureFlags();

    explicit KeeperFeatureFlags(std::string feature_flags_);

    /// backwards compatibility
    void fromApiVersion(KeeperApiVersion keeper_api_version);

    bool isEnabled(KeeperFeatureFlag feature) const;

    void setFeatureFlags(std::string feature_flags_);
    const std::string & getFeatureFlags() const;

    void enableFeatureFlag(KeeperFeatureFlag feature);
    void disableFeatureFlag(KeeperFeatureFlag feature);

    void logFlags(LoggerPtr log, DB::LogsLevel log_level = DB::LogsLevel::information) const;
private:
    std::string feature_flags;
};

}
