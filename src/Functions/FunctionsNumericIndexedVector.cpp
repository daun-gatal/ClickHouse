#include <Functions/FunctionFactory.h>

/// Include this last — see the reason inside
#include <Functions/FunctionsNumericIndexedVector.h>

namespace DB
{

REGISTER_FUNCTION(NumericIndexedVector)
{
    /// numericIndexedVectorBuild
    {
        FunctionDocumentation::Description description = R"(
Creates a NumericIndexedVector from a map. The map's keys represent the vector's index and map's value represents the vector's value.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorBuild(map)";
        FunctionDocumentation::Arguments arguments = {
            {"map", "A mapping from index to value.", {"Map"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a NumericIndexedVector object.", {"AggregateFunction"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
SELECT numericIndexedVectorBuild(mapFromArrays([1, 2, 3], [10, 20, 30])) AS res, toTypeName(res);
            )",
            R"(
┌─res─┬─toTypeName(res)────────────────────────────────────────────┐
│     │ AggregateFunction(groupNumericIndexedVector, UInt8, UInt8) │
└─────┴────────────────────────────────────────────────────────────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorBuild>(documentation);
    }
    /// numericIndexedVectorGetValue
    {
        FunctionDocumentation::Description description = R"(
Retrieves the value corresponding to a specified index from a numericIndexedVector.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorGetValue(v, i)";
        FunctionDocumentation::Arguments arguments = {
            {"v", "", {"numericIndexedVector"}},
            {"i", "The index for which the value is to be retrieved.", {"(U)Int*"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"A numeric value with the same type as the value type of NumericIndexedVector.", {"(U)Int*", "Float*"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
SELECT numericIndexedVectorGetValue(numericIndexedVectorBuild(mapFromArrays([1, 2, 3], [10, 20, 30])), 3) AS res;
            )",
            R"(
┌─res─┐
│  30 │
└─────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorGetValueImpl>(documentation);
    }
    /// numericIndexedVectorCardinality
    {
        FunctionDocumentation::Description description = R"(
Returns the cardinality (number of unique indexes) of the numericIndexedVector.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorCardinality(v)";
        FunctionDocumentation::Arguments arguments = {
            {"v", "", {"numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns the number of unique indexes.", {"UInt64"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
SELECT numericIndexedVectorCardinality(numericIndexedVectorBuild(mapFromArrays([1, 2, 3], [10, 20, 30]))) AS res;
            )",
            R"(
┌─res─┐
│  3  │
└─────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorCardinality>(documentation);
    }
    /// numericIndexedVectorAllValueSum
    {
        FunctionDocumentation::Description description = R"(
Returns the sum of all values in the numericIndexedVector.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorAllValueSum(v)";
        FunctionDocumentation::Arguments arguments = {
            {"v", "", {"numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns the sum.", {"Float64"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
SELECT numericIndexedVectorAllValueSum(numericIndexedVectorBuild(mapFromArrays([1, 2, 3], [10, 20, 30]))) AS res;
            )",
            R"(
┌─res─┐
│  60 │
└─────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorAllValueSum>(documentation);
    }
    /// numericIndexedVectorShortDebugString
    {
        FunctionDocumentation::Description description = R"(
Returns internal information of the numericIndexedVector in JSON format.
This function is primarily used for debugging purposes.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorShortDebugString(v)";
        FunctionDocumentation::Arguments arguments = {
            {"v", "", {"numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a JSON string containing debug information.", {"String"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
SELECT numericIndexedVectorShortDebugString(numericIndexedVectorBuild(mapFromArrays([1, 2, 3], [10, 20, 30]))) AS res\G;
            )",
            R"(
Row 1:
──────
res: {"vector_type":"BSI","index_type":"char8_t","value_type":"char8_t","integer_bit_num":8,"fraction_bit_num":0,"zero_indexes_info":{"cardinality":"0"},"non_zero_indexes_info":{"total_cardinality":"3","all_value_sum":60,"number_of_bitmaps":"8","bitmap_info":{"cardinality":{"0":"0","1":"2","2":"2","3":"2","4":"2","5":"0","6":"0","7":"0"}}}}
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorShortDebugString>(documentation);
    }
    /// numericIndexedVectorToMap
    {
        FunctionDocumentation::Description description = R"(
Converts a numericIndexedVector to a map.
        )";
        FunctionDocumentation::Syntax syntax = "numericIndexedVectorToMap(v)";
        FunctionDocumentation::Arguments arguments = {
            {"v", "", {"numericIndexedVector"}}
        };
        FunctionDocumentation::ReturnedValue returned_value = {"Returns a map with index-value pairs.", {"Map"}};
        FunctionDocumentation::Examples examples = {
        {
            "Usage example",
            R"(
SELECT numericIndexedVectorToMap(numericIndexedVectorBuild(mapFromArrays([1, 2, 3], [10, 20, 30]))) AS res;
            )",
            R"(
┌─res──────────────┐
│ {1:10,2:20,3:30} │
└──────────────────┘
            )"
        }
        };
        FunctionDocumentation::IntroducedIn introduced_in = {25, 7};
        FunctionDocumentation::Category category = FunctionDocumentation::Category::NumericIndexedVector;
        FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};
        factory.registerFunction<FunctionNumericIndexedVectorToMap>(documentation);
    }
}
}
