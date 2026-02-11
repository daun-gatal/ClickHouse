#include <Functions/bitOr.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitOrImpl, NameBitOr, true, false>;
extern template class FunctionBinaryArithmeticWithConstants<BitOrImpl, NameBitOr, true, false>;
extern template class BinaryArithmeticOverloadResolver<BitOrImpl, NameBitOr, true, false>;

REGISTER_FUNCTION(BitOr)
{
    FunctionDocumentation::Description description = "Performs bitwise OR operation between two values.";
    FunctionDocumentation::Syntax syntax = "bitOr(a, b)";
    FunctionDocumentation::Arguments arguments = {
        {"a", "First value.", {"(U)Int*", "Float*"}},
        {"b", "Second value.", {"(U)Int*", "Float*"}},
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the result of bitwise operation `a OR b`"};
    FunctionDocumentation::Examples examples = {{"Usage example",
        R"(
CREATE TABLE bits
(
    `a` UInt8,
    `b` UInt8
)
ENGINE = Memory;

INSERT INTO bits VALUES (0, 0), (0, 1), (1, 0), (1, 1);

SELECT
    a,
    b,
    bitOr(a, b)
FROM bits;
        )",
        R"(
┌─a─┬─b─┬─bitOr(a, b)─┐
│ 0 │ 0 │           0 │
│ 0 │ 1 │           1 │
│ 1 │ 0 │           1 │
│ 1 │ 1 │           1 │
└───┴───┴─────────────┘
        )"}};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Bit;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionBitOr>(documentation);
}

}
