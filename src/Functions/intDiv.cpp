#include <Functions/intDiv.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

/// Suppress ALL implicit instantiation of the intDiv arithmetic classes.
/// The class bodies are explicitly instantiated in intDivHalf1.cpp.
extern template class FunctionBinaryArithmetic<DivideIntegralImpl, NameIntDiv, false>;
extern template class FunctionBinaryArithmeticWithConstants<DivideIntegralImpl, NameIntDiv, false>;
extern template class BinaryArithmeticOverloadResolver<DivideIntegralImpl, NameIntDiv, false>;

REGISTER_FUNCTION(IntDiv)
{
    FunctionDocumentation::Description description = R"(
Performs an integer division of two values `x` by `y`. In other words it
computes the quotient rounded down to the next smallest integer.

The result has the same width as the dividend (the first parameter).

An exception is thrown when dividing by zero, when the quotient does not fit
in the range of the dividend, or when dividing a minimal negative number by minus one.
    )";
    FunctionDocumentation::Syntax syntax = "intDiv(x, y)";
    FunctionDocumentation::Argument argument1 = {"x", "Left hand operand."};
    FunctionDocumentation::Argument argument2 = {"y", "Right hand operand."};
    FunctionDocumentation::Arguments arguments = {argument1, argument2};
    FunctionDocumentation::ReturnedValue returned_value = {"Result of integer division of `x` and `y`"};
    FunctionDocumentation::Example example1 = {"Integer division of two floats", "SELECT intDiv(toFloat64(1), 0.001) AS res, toTypeName(res)", R"(
┌──res─┬─toTypeName(intDiv(toFloat64(1), 0.001))─┐
│ 1000 │ Int64                                   │
└──────┴─────────────────────────────────────────┘
    )"};
    FunctionDocumentation::Example example2 = {
        "Quotient does not fit in the range of the dividend",
        R"(
SELECT
intDiv(1, 0.001) AS res,
toTypeName(res)
        )",
        R"(
Received exception from server (version 23.2.1):
Code: 153. DB::Exception: Received from localhost:9000. DB::Exception:
Cannot perform integer division, because it will produce infinite or too
large number: While processing intDiv(1, 0.001) AS res, toTypeName(res).
(ILLEGAL_DIVISION)
        )"
    };
    FunctionDocumentation::Examples examples = {example1, example2};
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionIntDiv>(documentation);
}

}
