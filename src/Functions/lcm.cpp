#include <Functions/lcm.h>
#include <Functions/FunctionFactory.h>


namespace DB
{

/// Suppress ALL implicit instantiation of the lcm arithmetic classes.
/// The class bodies are explicitly instantiated in lcmHalf1.cpp.
extern template class FunctionBinaryArithmetic<LCMImpl, NameLCM, false, false>;
extern template class FunctionBinaryArithmeticWithConstants<LCMImpl, NameLCM, false, false>;
extern template class BinaryArithmeticOverloadResolver<LCMImpl, NameLCM, false, false>;

REGISTER_FUNCTION(LCM)
{
    FunctionDocumentation::Description description = R"(
Returns the least common multiple of two values `x` and `y`.

An exception is thrown when dividing by zero or when dividing a minimal negative number by minus one.
    )";
    FunctionDocumentation::Syntax syntax = "lcm(x, y)";
    FunctionDocumentation::Arguments arguments =
    {
        {"x", "First integer.", {"(U)Int*"}},
        {"y", "Second integer.", {"(U)Int*"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the least common multiple of `x` and `y`.", {"(U)Int*"}};
    FunctionDocumentation::Examples example = {{"Usage example", "SELECT lcm(6, 8)", "24"}};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, example, introduced_in, categories};

    factory.registerFunction<FunctionLCM>(documentation);
}

}
