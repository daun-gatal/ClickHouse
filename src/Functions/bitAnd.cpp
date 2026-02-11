#include <Functions/bitAnd.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitAndImpl, NameBitAnd, true, false>;
extern template class FunctionBinaryArithmeticWithConstants<BitAndImpl, NameBitAnd, true, false>;
extern template class BinaryArithmeticOverloadResolver<BitAndImpl, NameBitAnd, true, false>;

REGISTER_FUNCTION(BitAnd)
{
    FunctionDocumentation::Description description = "Performs bitwise AND operation between two values.";
    FunctionDocumentation::Syntax syntax = "bitAnd(a, b)";
    FunctionDocumentation::Arguments arguments = {
        {"a", "First value.", {"(U)Int*", "Float*"}},
        {"b", "Second value.", {"(U)Int*", "Float*"}},
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the result of bitwise operation `a AND b`"};
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
    bitAnd(a, b)
FROM bits
        )",
        R"(
┌─a─┬─b─┬─bitAnd(a, b)─┐
│ 0 │ 0 │            0 │
│ 0 │ 1 │            0 │
│ 1 │ 0 │            0 │
│ 1 │ 1 │            1 │
└───┴───┴──────────────┘
        )"}};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Bit;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionBitAnd>(documentation);
}

}
