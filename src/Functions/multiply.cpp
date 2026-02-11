#include <Functions/multiply.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

/// Suppress ALL implicit instantiation of the multiply arithmetic classes.
/// The class bodies are explicitly instantiated in multiplyHalf1.cpp.
extern template class FunctionBinaryArithmetic<MultiplyImpl, NameMultiply>;
extern template class FunctionBinaryArithmeticWithConstants<MultiplyImpl, NameMultiply>;
extern template class BinaryArithmeticOverloadResolver<MultiplyImpl, NameMultiply>;

REGISTER_FUNCTION(Multiply)
{
    FunctionDocumentation::Description description = "Calculates the product of two values `x` and `y`.";
    FunctionDocumentation::Syntax syntax = "multiply(x, y)";
    FunctionDocumentation::Arguments arguments =
    {
        {"x", "factor.", {"(U)Int*", "Float*", "Decimal"}},
        {"y", "factor.", {"(U)Int*", "Float*", "Decimal"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the product of x and y"};
    FunctionDocumentation::Examples examples = {{"Multiplying two numbers", "SELECT multiply(5,5)", "25"}};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionMultiply>(documentation);
}

}
