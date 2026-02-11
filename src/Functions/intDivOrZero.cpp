#include <Functions/intDivOrZero.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

/// Suppress ALL implicit instantiation of the intDivOrZero arithmetic classes.
/// The class bodies are explicitly instantiated in intDivOrZeroHalf1.cpp.
extern template class FunctionBinaryArithmetic<DivideIntegralOrZeroImpl, NameIntDivOrZero>;
extern template class FunctionBinaryArithmeticWithConstants<DivideIntegralOrZeroImpl, NameIntDivOrZero>;
extern template class BinaryArithmeticOverloadResolver<DivideIntegralOrZeroImpl, NameIntDivOrZero>;

REGISTER_FUNCTION(IntDivOrZero)
{
    FunctionDocumentation::Description description = R"(
Same as `intDiv` but returns zero when dividing by zero or when dividing a
minimal negative number by minus one.
    )";
    FunctionDocumentation::Syntax syntax = "intDivOrZero(a, b)";
    FunctionDocumentation::Arguments arguments =
    {
        {"a", "Left hand operand.", {"(U)Int*"}},
        {"b", "Right hand operand.", {"(U)Int*"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Result of integer division of a and b, or zero."};
    FunctionDocumentation::Examples examples =
    {
        {"Integer division by zero", "SELECT intDivOrZero(1, 0)", "0"},
        {"Dividing a minimal negative number by minus 1", "SELECT intDivOrZero(0.05, -1)", "0"}
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionIntDivOrZero>(documentation);
}

}
