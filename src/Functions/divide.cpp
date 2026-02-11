#include <Functions/divide.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

/// Suppress ALL implicit instantiation of the divide arithmetic classes.
/// The class bodies are explicitly instantiated in divideHalf1.cpp.
extern template class FunctionBinaryArithmetic<DivideFloatingImpl, NameDivide>;
extern template class FunctionBinaryArithmeticWithConstants<DivideFloatingImpl, NameDivide>;
extern template class BinaryArithmeticOverloadResolver<DivideFloatingImpl, NameDivide>;

REGISTER_FUNCTION(Divide)
{
    FunctionDocumentation::Description description = R"(
Calculates the quotient of two values `a` and `b`. The result type is always [Float64](/sql-reference/data-types/float).
Integer division is provided by the `intDiv` function.

:::note
Division by `0` returns `inf`, `-inf`, or `nan`.
:::
    )";
    FunctionDocumentation::Syntax syntax = "divide(x, y)";
    FunctionDocumentation::Argument argument1 = {"x", "Dividend"};
    FunctionDocumentation::Argument argument2 = {"y", "Divisor"};
    FunctionDocumentation::Arguments arguments = {argument1, argument2};
    FunctionDocumentation::ReturnedValue returned_value = {"The quotient of x and y"};
    FunctionDocumentation::Example example1 = {"Dividing two numbers", "SELECT divide(25,5) AS quotient, toTypeName(quotient)", "5 Float64"};
    FunctionDocumentation::Example example2 = {"Dividing by zero", "SELECT divide(25,0)", "inf"};
    FunctionDocumentation::Examples examples = {example1, example2};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionDivide>(documentation);
}

}
