#include <Functions/bitRotateLeft.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitRotateLeftImpl, NameBitRotateLeft, true, false>;
extern template class FunctionBinaryArithmeticWithConstants<BitRotateLeftImpl, NameBitRotateLeft, true, false>;
extern template class BinaryArithmeticOverloadResolver<BitRotateLeftImpl, NameBitRotateLeft, true, false>;

REGISTER_FUNCTION(BitRotateLeft)
{
    FunctionDocumentation::Description description = "Rotate bits left by a certain number of positions. Bits that fall off wrap around to the right.";
    FunctionDocumentation::Syntax syntax = "bitRotateLeft(a, N)";
    FunctionDocumentation::Arguments arguments = {
        {"a", "A value to rotate.", {"(U)Int8/16/32/64"}},
        {"N", "The number of positions to rotate left.", {"UInt8/16/32/64"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the rotated value with type equal to that of `a`.", {"(U)Int8/16/32/64"}};
    FunctionDocumentation::Examples examples = {{"Usage example",
        R"(
SELECT 99 AS a, bin(a), bitRotateLeft(a, 2) AS a_rotated, bin(a_rotated);
        )",
        R"(
┌──a─┬─bin(a)───┬─a_rotated─┬─bin(a_rotated)─┐
│ 99 │ 01100011 │       141 │ 10001101       │
└────┴──────────┴───────────┴────────────────┘
        )"}
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Bit;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionBitRotateLeft>(documentation);
}

}
