#include <Functions/minus.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

/// Suppress ALL implicit instantiation of the minus arithmetic classes.
/// The class bodies are explicitly instantiated in minusHalf1.cpp.
extern template class FunctionBinaryArithmetic<MinusImpl, NameMinus>;
extern template class FunctionBinaryArithmeticWithConstants<MinusImpl, NameMinus>;
extern template class BinaryArithmeticOverloadResolver<MinusImpl, NameMinus>;

REGISTER_FUNCTION(Minus)
{
    FunctionDocumentation::Description description = R"(
Calculates the difference of two values `a` and `b`. The result is always signed.
Similar to plus, it is possible to subtract an integer from a date or date with time.
Additionally, subtraction between date with time is supported, resulting in the time difference between them.
    )";
    FunctionDocumentation::Syntax syntax = "minus(x, y)";
    FunctionDocumentation::Arguments arguments =
    {
        {"x", "Minuend."},
        {"y", "Subtrahend."}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"x minus y"};
    FunctionDocumentation::Examples examples = {
        {"Subtracting two numbers", "SELECT minus(10, 5)", "5"},
        {"Subtracting an integer and a date", "SELECT minus(toDate('2025-01-01'),5)", "2024-12-27"}
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionMinus>(documentation);
}

}
