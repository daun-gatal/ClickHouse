#include <Functions/divideOrNull.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

/// Suppress ALL implicit instantiation of the divideOrNull arithmetic classes.
/// The class bodies are explicitly instantiated in divideOrNullHalf1.cpp.
extern template class FunctionBinaryArithmetic<DivideFloatingOrNullImpl, NameDivideOrNull>;
extern template class FunctionBinaryArithmeticWithConstants<DivideFloatingOrNullImpl, NameDivideOrNull>;
extern template class BinaryArithmeticOverloadResolver<DivideFloatingOrNullImpl, NameDivideOrNull>;

REGISTER_FUNCTION(DivideOrNull)
{
    FunctionDocumentation::Description description = R"(
Same as `divide` but returns NULL when dividing by zero.
    )";
    FunctionDocumentation::Syntax syntax = "divideOrNull(x, y)";
    FunctionDocumentation::Argument argument1 = {"x", "Dividend"};
    FunctionDocumentation::Argument argument2 = {"y", "Divisor"};
    FunctionDocumentation::Arguments arguments = {argument1, argument2};
    FunctionDocumentation::ReturnedValue returned_value = {"The quotient of x and y, or NULL."};
    FunctionDocumentation::Example example1 = {"Dividing by zero", "SELECT divideOrNull(25, 0)", "\\N"};
    FunctionDocumentation::Examples examples = {example1};
    FunctionDocumentation::IntroducedIn introduced_in = {25, 5};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionDivideOrNull>(documentation);
}

}
