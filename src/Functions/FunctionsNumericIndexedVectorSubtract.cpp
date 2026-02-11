#include <Functions/FunctionFactory.h>

/// Include this last — see the reason inside
#include <Functions/FunctionsNumericIndexedVector.h>

namespace DB
{

REGISTER_FUNCTION(NumericIndexedVectorPointwiseSubtract)
{
    FunctionDocumentation::Description description = R"(
Performs pointwise subtraction between a numericIndexedVector and either another numericIndexedVector or a numeric constant.
        )";
    FunctionDocumentation::Syntax syntax = "numericIndexedVectorPointwiseSubtract(v1, v2)";
    FunctionDocumentation::Arguments arguments = {
        {"v1", "", {"numericIndexedVector"}},
        {"v2", "A numeric constant or numericIndexedVector object.", {"(U)Int*", "Float*", "numericIndexedVector"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns a new numericIndexedVector object.", {"numericIndexedVector"}};
    FunctionDocumentation::Examples examples = {
    {
        "Usage example",
        R"(
WITH
    numericIndexedVectorBuild(mapFromArrays([1, 2, 3], arrayMap(x -> toInt32(x), [10, 20, 30]))) AS vec1,
    numericIndexedVectorBuild(mapFromArrays([2, 3, 4], arrayMap(x -> toInt32(x), [10, 20, 30]))) AS vec2
SELECT
    numericIndexedVectorToMap(numericIndexedVectorPointwiseSubtract(vec1, vec2)) AS res1,
    numericIndexedVectorToMap(numericIndexedVectorPointwiseSubtract(vec1, 2)) AS res2;
            )",
        R"(
┌─res1───────────────────┬─res2────────────┐
│ {1:10,2:10,3:10,4:-30} │ {1:8,2:18,3:28} │
└────────────────────────┴─────────────────┘
            )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
    factory.registerFunction<FunctionNumericIndexedVectorPointwiseSubtract>(documentation);
}
}
