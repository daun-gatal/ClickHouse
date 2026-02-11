#include <Functions/intDivOrNull.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

/// Suppress ALL implicit instantiation of the intDivOrNull arithmetic classes.
/// The class bodies are explicitly instantiated in intDivOrNullHalf1.cpp.
extern template class FunctionBinaryArithmetic<DivideIntegralOrNullImpl, NameIntDivOrNull, false>;
extern template class FunctionBinaryArithmeticWithConstants<DivideIntegralOrNullImpl, NameIntDivOrNull, false>;
extern template class BinaryArithmeticOverloadResolver<DivideIntegralOrNullImpl, NameIntDivOrNull, false>;

REGISTER_FUNCTION(IntDivOrNull)
{
    FunctionDocumentation::Description description = R"(
Same as `intDiv` but returns NULL when dividing by zero or when dividing a
minimal negative number by minus one.
    )";
    FunctionDocumentation::Syntax syntax = "intDivOrNull(x, y)";
    FunctionDocumentation::Arguments arguments =
    {
        {"x", "Left hand operand.", {"(U)Int*"}},
        {"y", "Right hand operand.", {"(U)Int*"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Result of integer division of `x` and `y`, or NULL."};
    FunctionDocumentation::Examples examples =
    {
        {"Integer division by zero", "SELECT intDivOrNull(1, 0)", "\\N"},
        {"Dividing a minimal negative number by minus 1", "SELECT intDivOrNull(-9223372036854775808, -1)", "\\N"}
    };
    FunctionDocumentation::IntroducedIn introduced_in = {25, 5};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionIntDivOrNull>(documentation);
}
}
