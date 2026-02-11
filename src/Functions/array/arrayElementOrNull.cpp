#include <Functions/array/arrayElement.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

REGISTER_FUNCTION(ArrayElementOrNull)
{
    FunctionDocumentation::Description description_null = R"(
Gets the element of the provided array with index `n` where `n` can be any integer type.
If the index falls outside of the bounds of an array, `NULL` is returned instead of a default value.

:::note
Arrays in ClickHouse are one-indexed.
:::

Negative indexes are supported. In this case, it selects the corresponding element numbered from the end. For example, `arr[-1]` is the last item in the array.
)";
    FunctionDocumentation::Syntax syntax_null = "arrayElementOrNull(arrays)";
    FunctionDocumentation::Arguments arguments_null = {
        {"arrays", "Arbitrary number of array arguments.", {"Array"}}
    };
    FunctionDocumentation::ReturnedValue returned_value_null = {"Returns a single combined array from the provided array arguments.", {"Array(T)"}};
    FunctionDocumentation::Examples examples_null = {
        {"Usage example", "SELECT arrayElementOrNull(arr, 2) FROM (SELECT [1, 2, 3] AS arr)", "2"},
        {"Negative indexing", "SELECT arrayElementOrNull(arr, -1) FROM (SELECT [1, 2, 3] AS arr)", "3"},
        {"Index out of array bounds", "SELECT arrayElementOrNull(arr, 4) FROM (SELECT [1, 2, 3] AS arr)", "NULL"}
    };
    FunctionDocumentation::IntroducedIn introduced_in_null = {1, 1};
    FunctionDocumentation::Category category_null = FunctionDocumentation::Category::Array;
    FunctionDocumentation documentation_null = {description_null, syntax_null, arguments_null, {}, returned_value_null, examples_null, introduced_in_null, category_null};

    factory.registerFunction<FunctionArrayElement<ArrayElementExceptionMode::Null>>(documentation_null);
}

}
