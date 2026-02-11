#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace DB
{

enum class LoadBalancing : uint8_t
{
    /// among replicas with a minimum number of errors selected randomly
    RANDOM = 0,
    /// a replica is selected among the replicas with the minimum number of errors
    /// with the minimum number of distinguished characters in the replica name prefix and local hostname prefix
    NEAREST_HOSTNAME,
    /// just like NEAREST_HOSTNAME, but it count distinguished characters in a levenshtein distance manner
    HOSTNAME_LEVENSHTEIN_DISTANCE,
    // replicas with the same number of errors are accessed in the same order
    // as they are specified in the configuration.
    IN_ORDER,
    /// if first replica one has higher number of errors,
    ///   pick a random one from replicas with minimum number of errors
    FIRST_OR_RANDOM,
    // round robin across replicas with the same number of errors.
    ROUND_ROBIN,
};

/// Case-sensitive parse of LoadBalancing from string (expects uppercase names).
inline std::optional<LoadBalancing> loadBalancingFromString(std::string_view str)
{
    if (str == "RANDOM") return LoadBalancing::RANDOM;
    if (str == "NEAREST_HOSTNAME") return LoadBalancing::NEAREST_HOSTNAME;
    if (str == "HOSTNAME_LEVENSHTEIN_DISTANCE") return LoadBalancing::HOSTNAME_LEVENSHTEIN_DISTANCE;
    if (str == "IN_ORDER") return LoadBalancing::IN_ORDER;
    if (str == "FIRST_OR_RANDOM") return LoadBalancing::FIRST_OR_RANDOM;
    if (str == "ROUND_ROBIN") return LoadBalancing::ROUND_ROBIN;
    return std::nullopt;
}

}
