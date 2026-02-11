#include <Functions/plus.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

/// Suppress ALL implicit instantiation of the plus arithmetic classes.
/// The class bodies are explicitly instantiated in plusHalf1.cpp / plusHalf2.cpp.
extern template class FunctionBinaryArithmetic<PlusImpl, NamePlus>;
extern template class FunctionBinaryArithmeticWithConstants<PlusImpl, NamePlus>;
extern template class BinaryArithmeticOverloadResolver<PlusImpl, NamePlus>;

REGISTER_FUNCTION(Plus)
{
    FunctionDocumentation::Description description = R"(
Calculates the sum of two values `x` and `y`. Alias: `x + y` (operator).
It is possible to add an integer and a date or date with time. The former
operation increments the number of days in the date, the latter operation
increments the number of seconds in the date with time.
    )";
    FunctionDocumentation::Syntax syntax = "plus(x, y)";
    FunctionDocumentation::Argument argument1 = {"x", "Left hand operand."};
    FunctionDocumentation::Argument argument2 = {"y", "Right hand operand."};
    FunctionDocumentation::Arguments arguments = {argument1, argument2};
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the sum of x and y"};
    FunctionDocumentation::Example example1 = {"Adding two numbers", "SELECT plus(5,5)", "10"};
    FunctionDocumentation::Example example2 = {"Adding an integer and a date", "SELECT plus(toDate('2025-01-01'),5)", "2025-01-06"};
    FunctionDocumentation::Examples examples = {example1, example2};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionPlus>(documentation);
}

}
