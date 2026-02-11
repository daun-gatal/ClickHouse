#include <Functions/array/arrayElement.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

REGISTER_FUNCTION(ArrayElement)
{
    FunctionDocumentation::Description description = R"(
Gets the element of the provided array with index `n` where `n` can be any integer type.
If the index falls outside of the bounds of an array, it returns a default value (0 for numbers, an empty string for strings, etc.),
except for arguments of a non-constant array and a constant index 0. In this case there will be an error `Array indices are 1-based`.

:::note
Arrays in ClickHouse are one-indexed.
:::

Negative indexes are supported. In this case, the corresponding element is selected, numbered from the end. For example, `arr[-1]` is the last item in the array.

Operator `[n]` provides the same functionality.
    )";
    FunctionDocumentation::Syntax syntax = "arrayElement(arr, n)";
    FunctionDocumentation::Arguments arguments = {
        {"arr", "The array to search. [`Array(T)`](/sql-reference/data-types/array)."},
        {"n", "Position of the element to get. [`(U)Int*`](/sql-reference/data-types/int-uint)."}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns a single combined array from the provided array arguments", {"Array(T)"}};
    FunctionDocumentation::Examples examples = {
        {"Usage example", "SELECT arrayElement(arr, 2) FROM (SELECT [1, 2, 3] AS arr)", "2"},
        {"Negative indexing", "SELECT arrayElement(arr, -1) FROM (SELECT [1, 2, 3] AS arr)", "3"},
        {"Using [n] notation", "SELECT arr[2] FROM (SELECT [1, 2, 3] AS arr)", "2"},
        {"Index out of array bounds", "SELECT arrayElement(arr, 4) FROM (SELECT [1, 2, 3] AS arr)", "0"}
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::Array;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionArrayElement<ArrayElementExceptionMode::Zero>>(documentation);
}

}
