#pragma once

#include <string_view>

namespace DB
{

/// A query in prometheus can evaluate to one of the following four types:
enum class PrometheusQueryResultType
{
    SCALAR,
    STRING,
    INSTANT_VECTOR,
    RANGE_VECTOR,
};

inline std::string_view enumToString(PrometheusQueryResultType value)
{
    switch (value)
    {
        case PrometheusQueryResultType::SCALAR: return "SCALAR";
        case PrometheusQueryResultType::STRING: return "STRING";
        case PrometheusQueryResultType::INSTANT_VECTOR: return "INSTANT_VECTOR";
        case PrometheusQueryResultType::RANGE_VECTOR: return "RANGE_VECTOR";
    }
}

}
