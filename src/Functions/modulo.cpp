#include <Functions/modulo.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

/// Suppress ALL implicit instantiation of the modulo arithmetic classes.
/// The class bodies are explicitly instantiated in moduloHalf1.cpp.
extern template class FunctionBinaryArithmetic<ModuloImpl, NameModulo, false>;
extern template class FunctionBinaryArithmeticWithConstants<ModuloImpl, NameModulo, false>;
extern template class BinaryArithmeticOverloadResolver<ModuloImpl, NameModulo, false>;

REGISTER_FUNCTION(Modulo)
{
    FunctionDocumentation::Description description = R"(
    Calculates the remainder of the division of two values a by b.

    The result type is an integer if both inputs are integers. If one of the
    inputs is a floating-point number, the result type is Float64.

    The remainder is computed like in C++. Truncated division is used for
    negative numbers.

    An exception is thrown when dividing by zero or when dividing a minimal
    negative number by minus one.
    )";
    FunctionDocumentation::Syntax syntax = "modulo(a, b)";
    FunctionDocumentation::Argument argument1 = {"a", "The dividend"};
    FunctionDocumentation::Argument argument2 = {"b", "The divisor (modulus)"};
    FunctionDocumentation::Arguments arguments = {argument1, argument2};
    FunctionDocumentation::ReturnedValue returned_value = {"The remainder of a % b"};
    FunctionDocumentation::Example example1 = {"Usage example", "SELECT modulo(5, 2)", "1"};
    FunctionDocumentation::Examples examples = {example1};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};
    factory.registerFunction<FunctionModulo>(documentation);
    factory.registerAlias("mod", "modulo", FunctionFactory::Case::Insensitive);
}

}
