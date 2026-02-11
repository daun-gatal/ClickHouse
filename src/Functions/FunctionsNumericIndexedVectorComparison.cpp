#include <Functions/FunctionFactory.h>

/// Include this last — see the reason inside
#include <Functions/FunctionsNumericIndexedVector.h>

namespace DB
{

REGISTER_FUNCTION(NumericIndexedVectorComparison)
{
    /// numericIndexedVectorPointwiseEqual
    {
        FunctionDocumentation::Description description = R"(
Performs pointwise comparison between a numericIndexedVector and either another numericIndexedVector or a numeric constant.
The result is a numericIndexedVector containing the indices where the values are equal, with all corresponding values set to 1.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorPointwiseEqual(v1, v2)";
        FunctionDocumentation::Arguments arguments = {
            {"v1", "", {"numericIndexedVector"}},
            {"v2", "A numeric constant or numericIndexedVector object.", {"(U)Int*", "Float*", "numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a new numericIndexedVector object.", {"numericIndexedVector"}};
        FunctionDocumentation::Examples examples = {
        {"",
            R"(
with
    numericIndexedVectorBuild(mapFromArrays([1, 2, 3], arrayMap(x -> toFloat64(x), [10, 20, 30]))) as vec1,
    numericIndexedVectorBuild(mapFromArrays([2, 3, 4], arrayMap(x -> toFloat64(x), [20, 20, 30]))) as vec2
SELECT
    numericIndexedVectorToMap(numericIndexedVectorPointwiseEqual(vec1, vec2)) AS res1,
    numericIndexedVectorToMap(numericIndexedVectorPointwiseEqual(vec1, 20)) AS res2;
            )",
            R"(
┌─res1──┬─res2──┐
│ {2:1} │ {2:1} │
└───────┴───────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorPointwiseEqual>(documentation);
    }
    /// numericIndexedVectorPointwiseNotEqual
    {
        FunctionDocumentation::Description description = R"(
Performs pointwise comparison between a numericIndexedVector and either another numericIndexedVector or a numeric constant.
The result is a numericIndexedVector containing the indices where the values are not equal, with all corresponding values set to 1.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorPointwiseNotEqual(v1, v2)";
        FunctionDocumentation::Arguments arguments = {
            {"v1", "", {"numericIndexedVector"}},
            {"v2", "A numeric constant or numericIndexedVector object.", {"(U)Int*", "Float*", "numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a new numericIndexedVector object.", {"numericIndexedVector"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
with
    numericIndexedVectorBuild(mapFromArrays([1, 2, 3], arrayMap(x -> toFloat64(x), [10, 20, 30]))) as vec1,
    numericIndexedVectorBuild(mapFromArrays([2, 3, 4], arrayMap(x -> toFloat64(x), [20, 20, 30]))) as vec2
SELECT
    numericIndexedVectorToMap(numericIndexedVectorPointwiseNotEqual(vec1, vec2)) AS res1,
    numericIndexedVectorToMap(numericIndexedVectorPointwiseNotEqual(vec1, 20)) AS res2;
            )",
            R"(
┌─res1──────────┬─res2──────┐
│ {1:1,3:1,4:1} │ {1:1,3:1} │
└───────────────┴───────────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorPointwiseNotEqual>(documentation);
    }
    /// numericIndexedVectorPointwiseLess
    {
        FunctionDocumentation::Description description = R"(
Performs pointwise comparison between a numericIndexedVector and either another numericIndexedVector or a numeric constant.
The result is a numericIndexedVector containing the indices where the first vector's value is less than the second vector's value, with all corresponding values set to 1.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorPointwiseLess(v1, v2)";
        FunctionDocumentation::Arguments arguments = {
            {"v1", "", {"numericIndexedVector"}},
            {"v2", "A numeric constant or numericIndexedVector object.", {"(U)Int*", "Float*", "numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a new numericIndexedVector object.", {"numericIndexedVector"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
with
    numericIndexedVectorBuild(mapFromArrays([1, 2, 3], arrayMap(x -> toFloat64(x), [10, 20, 30]))) as vec1,
    numericIndexedVectorBuild(mapFromArrays([2, 3, 4], arrayMap(x -> toFloat64(x), [20, 40, 30]))) as vec2
SELECT
    numericIndexedVectorToMap(numericIndexedVectorPointwiseLess(vec1, vec2)) AS res1,
    numericIndexedVectorToMap(numericIndexedVectorPointwiseLess(vec1, 20)) AS res2;
            )",
            R"(
┌─res1──────┬─res2──┐
│ {3:1,4:1} │ {1:1} │
└───────────┴───────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorPointwiseLess>(documentation);
    }
    /// numericIndexedVectorPointwiseLessEqual
    {
        FunctionDocumentation::Description description = R"(
Performs pointwise comparison between a numericIndexedVector and either another numericIndexedVector or a numeric constant.
The result is a numericIndexedVector containing the indices where the first vector's value is less than or equal to the second vector's value, with all corresponding values set to 1.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorPointwiseLessEqual(v1, v2)";
        FunctionDocumentation::Arguments arguments = {
            {"v1", "", {"numericIndexedVector"}},
            {"v2", "A numeric constant or numericIndexedVector object", {"(U)Int*", "Float*", "numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a new numericIndexedVector object.", {"numericIndexedVector"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
with
    numericIndexedVectorBuild(mapFromArrays([1, 2, 3], arrayMap(x -> toFloat64(x), [10, 20, 30]))) as vec1,
    numericIndexedVectorBuild(mapFromArrays([2, 3, 4], arrayMap(x -> toFloat64(x), [20, 40, 30]))) as vec2
SELECT
    numericIndexedVectorToMap(numericIndexedVectorPointwiseLessEqual(vec1, vec2)) AS res1,
    numericIndexedVectorToMap(numericIndexedVectorPointwiseLessEqual(vec1, 20)) AS res2;
            )",
            R"(
┌─res1──────────┬─res2──────┐
│ {2:1,3:1,4:1} │ {1:1,2:1} │
└───────────────┴───────────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorPointwiseLessEqual>(documentation);
    }
    /// numericIndexedVectorPointwiseGreater
    {
        FunctionDocumentation::Description description = R"(
Performs pointwise comparison between a numericIndexedVector and either another numericIndexedVector or a numeric constant.
The result is a numericIndexedVector containing the indices where the first vector's value is greater than the second vector's value, with all corresponding values set to 1.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorPointwiseGreater(v1, v2)";
        FunctionDocumentation::Arguments arguments = {
            {"v1", "", {"numericIndexedVector"}},
            {"v2", "A numeric constant or numericIndexedVector object.", {"(U)Int*", "Float*", "numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a new numericIndexedVector object.", {"numericIndexedVector"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
with
    numericIndexedVectorBuild(mapFromArrays([1, 2, 3], arrayMap(x -> toFloat64(x), [10, 20, 50]))) as vec1,
    numericIndexedVectorBuild(mapFromArrays([2, 3, 4], arrayMap(x -> toFloat64(x), [20, 40, 30]))) as vec2
SELECT
    numericIndexedVectorToMap(numericIndexedVectorPointwiseGreater(vec1, vec2)) AS res1,
    numericIndexedVectorToMap(numericIndexedVectorPointwiseGreater(vec1, 20)) AS res2;
            )",
            R"(
┌─res1──────┬─res2──┐
│ {1:1,3:1} │ {3:1} │
└───────────┴───────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorPointwiseGreater>(documentation);
    }
    /// numericIndexedVectorPointwiseGreaterEqual
    {
        FunctionDocumentation::Description description = R"(
Performs pointwise comparison between a numericIndexedVector and either another numericIndexedVector or a numeric constant.
The result is a numericIndexedVector containing the indices where the first vector's value is greater than or equal to the second vector's value, with all corresponding values set to 1.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorPointwiseGreaterEqual(v1, v2)";
        FunctionDocumentation::Arguments arguments = {
            {"v1", "", {"numericIndexedVector"}},
            {"v2", "A numeric constant or numericIndexedVector object.", {"(U)Int*", "Float*", "numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a new numericIndexedVector object.", {"numericIndexedVector"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
with
    numericIndexedVectorBuild(mapFromArrays([1, 2, 3], arrayMap(x -> toFloat64(x), [10, 20, 50]))) as vec1,
    numericIndexedVectorBuild(mapFromArrays([2, 3, 4], arrayMap(x -> toFloat64(x), [20, 40, 30]))) as vec2
SELECT
    numericIndexedVectorToMap(numericIndexedVectorPointwiseGreaterEqual(vec1, vec2)) AS res1,
    numericIndexedVectorToMap(numericIndexedVectorPointwiseGreaterEqual(vec1, 20)) AS res2;
            )",
            R"(
┌─res1──────────┬─res2──────┐
│ {1:1,2:1,3:1} │ {2:1,3:1} │
└───────────────┴───────────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorPointwiseGreaterEqual>(documentation);
    }
}
}
