#include <Functions/bitHammingDistance.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitHammingDistanceImpl, NameBitHammingDistance>;
extern template class FunctionBinaryArithmeticWithConstants<BitHammingDistanceImpl, NameBitHammingDistance>;
extern template class BinaryArithmeticOverloadResolver<BitHammingDistanceImpl, NameBitHammingDistance>;

REGISTER_FUNCTION(BitHammingDistance)
{
    FunctionDocumentation::Description description = R"(
Returns the [Hamming Distance](https://en.wikipedia.org/wiki/Hamming_distance) between the bit representations of two numbers.
Can be used with [`SimHash`](../../sql-reference/functions/hash-functions.md#ngramSimHash) functions for detection of semi-duplicate strings.
The smaller the distance, the more similar the strings are.
)";
    FunctionDocumentation::Syntax syntax = "bitHammingDistance(x, y)";
    FunctionDocumentation::Arguments arguments = {
        {"x", "First number for Hamming distance calculation.", {"(U)Int*", "Float*"}},
        {"y", "Second number for Hamming distance calculation.", {"(U)Int*", "Float*"}},
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the hamming distance between `x` and `y`", {"UInt8"}};
    FunctionDocumentation::Examples examples = {{"Usage example", "SELECT bitHammingDistance(111, 121);",
        R"(
┌─bitHammingDistance(111, 121)─┐
│                            3 │
└──────────────────────────────┘
        )"}
    };
    FunctionDocumentation::IntroducedIn introduced_in = {21, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Bit;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionBitHammingDistance>(documentation);
}
}
