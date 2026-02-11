#include <Functions/gcd.h>
#include <Functions/FunctionFactory.h>


namespace DB
{

/// Suppress ALL implicit instantiation of the gcd arithmetic classes.
/// The class bodies are explicitly instantiated in gcdHalf1.cpp.
extern template class FunctionBinaryArithmetic<GCDImpl, NameGCD, false, false>;
extern template class FunctionBinaryArithmeticWithConstants<GCDImpl, NameGCD, false, false>;
extern template class BinaryArithmeticOverloadResolver<GCDImpl, NameGCD, false, false>;

REGISTER_FUNCTION(GCD)
{
    FunctionDocumentation::Description description = R"(
    Returns the greatest common divisor of two values a and b.

    An exception is thrown when dividing by zero or when dividing a minimal
    negative number by minus one.
    )";
    FunctionDocumentation::Syntax syntax = "gcd(x, y)";
    FunctionDocumentation::Argument argument1 = {"x", "First integer"};
    FunctionDocumentation::Argument argument2 = {"y", "Second integer"};
    FunctionDocumentation::Arguments arguments = {argument1, argument2};
    FunctionDocumentation::ReturnedValue returned_value = {"The greatest common divisor of `x` and `y`."};
    FunctionDocumentation::Example example1 = {"Usage example", "SELECT gcd(12, 18)", "6"};
    FunctionDocumentation::Examples examples = {example1};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionGCD>(documentation);
}

}
