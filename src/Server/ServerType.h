#pragma once

#include <base/types.h>
#include <string_view>
#include <stdexcept>
#include <unordered_set>

namespace DB
{

class ServerType
{
public:
    enum Type
    {
        TCP_WITH_PROXY,
        TCP_SECURE,
        TCP_SSH,
        TCP,
        HTTP,
        HTTPS,
        MYSQL,
        GRPC,
        ARROW_FLIGHT,
        POSTGRESQL,
        PROMETHEUS,
        CUSTOM,
        INTERSERVER_HTTP,
        INTERSERVER_HTTPS,
        QUERIES_ALL,
        QUERIES_DEFAULT,
        QUERIES_CUSTOM,
        CLOUD,
        END
    };

    using Types = std::unordered_set<Type>;
    using CustomNames = std::unordered_set<String>;

    ServerType() = default;

    explicit ServerType(
        Type type_,
        const std::string & custom_name_ = "",
        const Types & exclude_types_ = {},
        const CustomNames exclude_custom_names_ = {})
        : type(type_),
          custom_name(custom_name_),
          exclude_types(exclude_types_),
          exclude_custom_names(exclude_custom_names_) {}

    static const char * serverTypeToString(Type type);

    /// Checks whether provided in the arguments type should be started or stopped based on current server type.
    bool shouldStart(Type server_type, const std::string & server_custom_name = "") const;
    bool shouldStop(const std::string & port_name) const;

    Type type;
    std::string custom_name;

    Types exclude_types;
    CustomNames exclude_custom_names;
};

inline std::string_view enumToString(ServerType::Type type)
{
    switch (type)
    {
        case ServerType::TCP_WITH_PROXY:    return "TCP_WITH_PROXY";
        case ServerType::TCP_SECURE:        return "TCP_SECURE";
        case ServerType::TCP_SSH:           return "TCP_SSH";
        case ServerType::TCP:               return "TCP";
        case ServerType::HTTP:              return "HTTP";
        case ServerType::HTTPS:             return "HTTPS";
        case ServerType::MYSQL:             return "MYSQL";
        case ServerType::GRPC:              return "GRPC";
        case ServerType::ARROW_FLIGHT:      return "ARROW_FLIGHT";
        case ServerType::POSTGRESQL:        return "POSTGRESQL";
        case ServerType::PROMETHEUS:        return "PROMETHEUS";
        case ServerType::CUSTOM:            return "CUSTOM";
        case ServerType::INTERSERVER_HTTP:  return "INTERSERVER_HTTP";
        case ServerType::INTERSERVER_HTTPS: return "INTERSERVER_HTTPS";
        case ServerType::QUERIES_ALL:       return "QUERIES_ALL";
        case ServerType::QUERIES_DEFAULT:   return "QUERIES_DEFAULT";
        case ServerType::QUERIES_CUSTOM:    return "QUERIES_CUSTOM";
        case ServerType::CLOUD:             return "CLOUD";
        case ServerType::END:               return "END";
    }
    throw std::runtime_error("Unknown ServerType::Type");
}

}
