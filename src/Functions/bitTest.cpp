#include <Functions/bitTest.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitTestImpl, NameBitTest, true, false>;
extern template class FunctionBinaryArithmeticWithConstants<BitTestImpl, NameBitTest, true, false>;
extern template class BinaryArithmeticOverloadResolver<BitTestImpl, NameBitTest, true, false>;

REGISTER_FUNCTION(BitTest)
{
    FunctionDocumentation::Description description = "Takes any number and converts it into [binary form](https://en.wikipedia.org/wiki/Binary_number), then returns the value of the bit at a specified position. Counting is done right-to-left, starting at 0.";
    FunctionDocumentation::Syntax syntax = "bitTest(a, i)";
    FunctionDocumentation::Arguments arguments = {
        {"a", "Number to convert.", {"(U)Int8/16/32/64", "Float*"}},
        {"i", "Position of the bit to return.", {"(U)Int8/16/32/64", "Float*"}},
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the value of the bit at position `i` in the binary representation of `a`", {"UInt8"}};
    FunctionDocumentation::Examples examples = {{"Usage example", "SELECT bin(2), bitTest(2, 1);",
        R"(
┌─bin(2)───┬─bitTest(2, 1)─┐
│ 00000010 │             1 │
└──────────┴───────────────┘
        )"}};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Bit;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionBitTest>(documentation);
}

}
