#include <Functions/bitXor.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitXorImpl, NameBitXor, true, false>;
extern template class FunctionBinaryArithmeticWithConstants<BitXorImpl, NameBitXor, true, false>;
extern template class BinaryArithmeticOverloadResolver<BitXorImpl, NameBitXor, true, false>;

REGISTER_FUNCTION(BitXor)
{
    FunctionDocumentation::Description description = "Performs bitwise ""exclusive or"" (XOR) operation between two values.";
    FunctionDocumentation::Syntax syntax = "bitXor(a, b)";
    FunctionDocumentation::Arguments arguments = {
        {"a", "First value.", {"(U)Int*", "Float*"}},
        {"b", "Second value.", {"(U)Int*", "Float*"}},
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the result of bitwise operation `a XOR b`"};
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
    bitXor(a, b)
FROM bits;
        )",
        R"(
┌─a─┬─b─┬─bitXor(a, b)─┐
│ 0 │ 0 │            0 │
│ 0 │ 1 │            1 │
│ 1 │ 0 │            1 │
│ 1 │ 1 │            0 │
└───┴───┴──────────────┘
        )"}};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Bit;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionBitXor>(documentation);
}

}
