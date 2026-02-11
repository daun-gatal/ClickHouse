#include <Functions/FunctionFactory.h>
#include <Functions/FunctionsRound.h>


namespace DB
{

REGISTER_FUNCTION(Round)
{
    {
        FunctionDocumentation::Description description = R"(
Returns the largest rounded number less than or equal to `x`, where the rounded number is a multiple of `1 / 10 * N`, or the nearest number of the appropriate data type if `1 / 10 * N` isn't exact.

Integer arguments may be rounded with a negative `N` argument.
With non-negative `N` the function returns `x`.

If rounding causes an overflow (for example, `floor(-128, -1)`), the result is undefined.
)";
        FunctionDocumentation::Syntax syntax = "floor(x[, N])";
        FunctionDocumentation::Arguments arguments = {
            {"x", "The value to round.", {"Float*", "Decimal*", "(U)Int*"}},
            {"N", "Optional. The number of decimal places to round to. Defaults to zero, which means rounding to an integer. Can be negative.", {"(U)Int*"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a rounded number of the same type as `x`.", {"Float*", "Decimal*", "(U)Int*"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            "SELECT floor(123.45, 1) AS rounded",
            R"(
┌─rounded─┐
│   123.4 │
└─────────┘
            )"
        },
        {
            "Negative precision",
            "SELECT floor(123.45, -1)",
            R"(
┌─floor(123.45, -1)─┐
│               120 │
└───────────────────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::Rounding;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionFloor>(documentation, FunctionFactory::Case::Insensitive);
    }

    {
        FunctionDocumentation::Description description = R"(
Like [`floor`](#floor) but returns the smallest rounded number greater than or equal to `x`.
If rounding causes an overflow (for example, `ceiling(255, -1)`), the result is undefined.
)";
        FunctionDocumentation::Syntax syntax = "ceiling(x[, N])";
        FunctionDocumentation::Arguments arguments = {
            {"x", "The value to round.", {"Float*", "Decimal*", "(U)Int*"}},
            {"N", "Optional. The number of decimal places to round to. Defaults to zero, which means rounding to an integer. Can be negative.", {"(U)Int*"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a rounded number of the same type as `x`.", {"Float*", "Decimal*", "(U)Int*"}};
        FunctionDocumentation::Examples examples = {
            {
                "Basic usage",
                "SELECT ceiling(123.45, 1) AS rounded",
                R"(
┌─rounded─┐
│   123.5 │
└─────────┘
            )"
            },
            {
                "Negative precision",
                "SELECT ceiling(123.45, -1)",
                R"(
┌─ceiling(123.45, -1)─┐
│                 130 │
└─────────────────────┘
            )"
            }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::Rounding;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionCeil>(documentation, FunctionFactory::Case::Insensitive);
    }

    {
        FunctionDocumentation::Description description = R"(
Like [`floor`](#floor) but returns the rounded number with the largest absolute value less than or equal to that of `x`.
)";
        FunctionDocumentation::Syntax syntax = "truncate(x[, N])";
        FunctionDocumentation::Arguments arguments = {
            {"x", "The value to round.", {"Float*", "Decimal*", "(U)Int*"}},
            {"N", "Optional. The number of decimal places to round to. Defaults to zero, which means rounding to an integer.", {"(U)Int*"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a rounded number of the same type as `x`.", {"Float*", "Decimal*", "(U)Int*"}};
        FunctionDocumentation::Examples examples = {
            {"Basic usage", "SELECT truncate(123.499, 1) AS res;", "┌───res─┐\n│ 123.4 │\n└───────┘"}
        };
        FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::Rounding;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionTrunc>(documentation, FunctionFactory::Case::Insensitive);
    }

    /// Compatibility aliases.
    factory.registerAlias("ceiling", "ceil", FunctionFactory::Case::Insensitive);
    factory.registerAlias("truncate", "trunc", FunctionFactory::Case::Insensitive);
}

}
