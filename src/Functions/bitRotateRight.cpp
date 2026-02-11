#include <Functions/bitRotateRight.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitRotateRightImpl, NameBitRotateRight, true, false>;
extern template class FunctionBinaryArithmeticWithConstants<BitRotateRightImpl, NameBitRotateRight, true, false>;
extern template class BinaryArithmeticOverloadResolver<BitRotateRightImpl, NameBitRotateRight, true, false>;

REGISTER_FUNCTION(BitRotateRight)
{
    FunctionDocumentation::Description description = "Rotate bits right by a certain number of positions. Bits that fall off wrap around to the left.";
    FunctionDocumentation::Syntax syntax = "bitRotateRight(a, N)";
    FunctionDocumentation::Arguments arguments = {
        {"a", "A value to rotate.", {"(U)Int8/16/32/64"}},
        {"N", "The number of positions to rotate right.", {"UInt8/16/32/64"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the rotated value with type equal to that of `a`.", {"(U)Int8/16/32/64"}};
    FunctionDocumentation::Examples examples = {{"Usage example",
        R"(
SELECT 99 AS a, bin(a), bitRotateRight(a, 2) AS a_rotated, bin(a_rotated);
        )",
        R"(
┌──a─┬─bin(a)───┬─a_rotated─┬─bin(a_rotated)─┐
│ 99 │ 01100011 │       216 │ 11011000       │
└────┴──────────┴───────────┴────────────────┘
        )"}
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Bit;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionBitRotateRight>(documentation);
}

}
