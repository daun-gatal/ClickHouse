#include <Functions/greatest.h>
#include <Functions/FunctionFactory.h>


namespace DB
{

/// Suppress ALL implicit instantiation of the greatest arithmetic class.
/// The class body is explicitly instantiated in greatestHalf1.cpp.
extern template class FunctionBinaryArithmetic<GreatestImpl, NameGreatest>;

REGISTER_FUNCTION(Greatest)
{
    FunctionDocumentation::Description description = R"(
Returns the greatest value among the arguments.
`NULL` arguments are ignored.

- For arrays, returns the lexicographically greatest array.
- For `DateTime` types, the result type is promoted to the largest type (e.g., `DateTime64` if mixed with `DateTime32`).

:::note Use setting `least_greatest_legacy_null_behavior` to change `NULL` behavior
Version [24.12](/whats-new/changelog/2024#a-id2412a-clickhouse-release-2412-2024-12-19) introduced a backwards-incompatible change such that `NULL` values are ignored, while previously it returned `NULL` if one of the arguments was `NULL`.
To retain the previous behavior, set setting `least_greatest_legacy_null_behavior` (default: `false`) to `true`.
:::
    )";
    FunctionDocumentation::Syntax syntax = "greatest(x1[, x2, ...])";
    FunctionDocumentation::Arguments arguments = {
        {"x1[, x2, ...]", "One or multiple values to compare. All arguments must be of comparable types.", {"Any"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the greatest value among the arguments, promoted to the largest compatible type.", {"Any"}};
    FunctionDocumentation::Examples examples = {
    {
        "Numeric types",
        R"(
SELECT greatest(1, 2, toUInt8(3), 3.) AS result, toTypeName(result) AS type;
-- The type returned is a Float64 as the UInt8 must be promoted to 64 bit for the comparison.
        )",
        R"(
┌─result─┬─type────┐
│      3 │ Float64 │
└────────┴─────────┘
        )"
    },
    {
        "Arrays",
        R"(
SELECT greatest(['hello'], ['there'], ['world']);
        )",
        R"(
┌─greatest(['hello'], ['there'], ['world'])─┐
│ ['world']                                 │
└───────────────────────────────────────────┘
        )"
    },
    {
        "DateTime types",
        R"(
SELECT greatest(toDateTime32(now() + toIntervalDay(1)), toDateTime64(now(), 3));
-- The type returned is a DateTime64 as the DateTime32 must be promoted to 64 bit for the comparison.
        )",
        R"(
┌─greatest(toD⋯(now(), 3))─┐
│  2025-05-28 15:50:53.000 │
└──────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Conditional;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<LeastGreatestOverloadResolver<LeastGreatest::Greatest, FunctionGreatest>>(documentation, FunctionFactory::Case::Insensitive);
}

}
