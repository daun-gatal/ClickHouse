#include <Functions/moduloOrZero.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<ModuloOrZeroImpl, NameModuloOrZero>;
extern template class FunctionBinaryArithmeticWithConstants<ModuloOrZeroImpl, NameModuloOrZero>;
extern template class BinaryArithmeticOverloadResolver<ModuloOrZeroImpl, NameModuloOrZero>;

REGISTER_FUNCTION(ModuloOrZero)
{
    FunctionDocumentation::Description description = R"(
Like modulo but returns zero when the divisor is zero, as opposed to an
exception with the modulo function.
    )";
    FunctionDocumentation::Syntax syntax = "moduloOrZero(a, b)";
    FunctionDocumentation::Arguments arguments =
    {
        {"a", "The dividend.", {"(U)Int*", "Float*"}},
        {"b", "The divisor (modulus).", {"(U)Int*", "Float*"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the remainder of a % b, or `0` when the divisor is `0`."};
    FunctionDocumentation::Examples examples = {{"Usage example", "SELECT moduloOrZero(5, 0)", "0"}};
    FunctionDocumentation::IntroducedIn introduced_in = {20, 3};
    FunctionDocumentation::Category categories = FunctionDocumentation::Category::Arithmetic;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, categories};

    factory.registerFunction<FunctionModuloOrZero>(documentation);
}

}
