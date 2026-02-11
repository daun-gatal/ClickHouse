#include <Functions/bitShiftRight.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitShiftRightImpl, NameBitShiftRight, true, false>;
extern template class FunctionBinaryArithmeticWithConstants<BitShiftRightImpl, NameBitShiftRight, true, false>;
extern template class BinaryArithmeticOverloadResolver<BitShiftRightImpl, NameBitShiftRight, true, false>;

REGISTER_FUNCTION(BitShiftRight)
{
        FunctionDocumentation::Description description = R"(
Shifts the binary representation of a value to the right by a specified number of bit positions.

A `FixedString` or a `String` is treated as a single multibyte value.

Bits of a `FixedString` value are lost as they are shifted out.
On the contrary, a `String` value is extended with additional bytes, so no bits are lost.
)";
    FunctionDocumentation::Syntax syntax = "bitShiftRight(a, N)";
    FunctionDocumentation::Arguments arguments = {
        {"a", "A value to shift.", {"(U)Int*", "String", "FixedString"}},
        {"N", "The number of positions to shift.", {"UInt8/16/32/64"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the shifted value with type equal to that of `a`."};
    FunctionDocumentation::Examples examples = {{"Usage example with binary encoding",
        R"(
SELECT 101 AS a, bin(a), bitShiftRight(a, 2) AS a_shifted, bin(a_shifted);
        )",
        R"(
┌───a─┬─bin(101)─┬─a_shifted─┬─bin(bitShiftRight(101, 2))─┐
│ 101 │ 01100101 │        25 │ 00011001                   │
└─────┴──────────┴───────────┴────────────────────────────┘
        )"},
        {"Usage example with hexadecimal encoding", R"(
SELECT 'abc' AS a, hex(a), bitShiftLeft(a, 4) AS a_shifted, hex(a_shifted);
        )",
        R"(
┌─a───┬─hex('abc')─┬─a_shifted─┬─hex(bitShiftRight('abc', 12))─┐
│ abc │ 616263     │           │ 0616                          │
└─────┴────────────┴───────────┴───────────────────────────────┘
        )"},
{"Usage example with Fixed String encoding", R"(
SELECT toFixedString('abc', 3) AS a, hex(a), bitShiftRight(a, 12) AS a_shifted, hex(a_shifted);
        )",
R"(
┌─a───┬─hex(toFixedString('abc', 3))─┬─a_shifted─┬─hex(bitShiftRight(toFixedString('abc', 3), 12))─┐
│ abc │ 616263                       │           │ 000616                                          │
└─────┴──────────────────────────────┴───────────┴─────────────────────────────────────────────────┘
        )"},
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Bit;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionBitShiftRight>(documentation);
}

}
