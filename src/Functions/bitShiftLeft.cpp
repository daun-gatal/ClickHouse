#include <Functions/bitShiftLeft.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitShiftLeftImpl, NameBitShiftLeft, true, false>;
extern template class FunctionBinaryArithmeticWithConstants<BitShiftLeftImpl, NameBitShiftLeft, true, false>;
extern template class BinaryArithmeticOverloadResolver<BitShiftLeftImpl, NameBitShiftLeft, true, false>;

REGISTER_FUNCTION(BitShiftLeft)
{
    FunctionDocumentation::Description description = R"(
Shifts the binary representation of a value to the left by a specified number of bit positions.

A `FixedString` or a `String` is treated as a single multibyte value.

Bits of a `FixedString` value are lost as they are shifted out.
On the contrary, a `String` value is extended with additional bytes, so no bits are lost.
)";
    FunctionDocumentation::Syntax syntax = "bitShiftLeft(a, N)";
    FunctionDocumentation::Arguments arguments = {
        {"a", "A value to shift.", {"(U)Int*", "String", "FixedString"}},
        {"N", "The number of positions to shift.", {"UInt8/16/32/64"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the shifted value with type equal to that of `a`."};
    FunctionDocumentation::Examples examples = {{"Usage example with binary encoding",
        R"(
SELECT 99 AS a, bin(a), bitShiftLeft(a, 2) AS a_shifted, bin(a_shifted);
        )",
        R"(
┌──a─┬─bin(99)──┬─a_shifted─┬─bin(bitShiftLeft(99, 2))─┐
│ 99 │ 01100011 │       140 │ 10001100                 │
└────┴──────────┴───────────┴──────────────────────────┘
        )"},
        {"Usage example with hexadecimal encoding", R"(
SELECT 'abc' AS a, hex(a), bitShiftLeft(a, 4) AS a_shifted, hex(a_shifted);
        )",
        R"(
┌─a───┬─hex('abc')─┬─a_shifted─┬─hex(bitShiftLeft('abc', 4))─┐
│ abc │ 616263     │ &0        │ 06162630                    │
└─────┴────────────┴───────────┴─────────────────────────────┘
        )"},
{"Usage example with Fixed String encoding", R"(
SELECT toFixedString('abc', 3) AS a, hex(a), bitShiftLeft(a, 4) AS a_shifted, hex(a_shifted);
        )",
R"(
┌─a───┬─hex(toFixedString('abc', 3))─┬─a_shifted─┬─hex(bitShiftLeft(toFixedString('abc', 3), 4))─┐
│ abc │ 616263                       │ &0        │ 162630                                        │
└─────┴──────────────────────────────┴───────────┴───────────────────────────────────────────────┘
        )"},
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Bit;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionBitShiftLeft>(documentation);
}

}
