#include <Functions/FunctionFactory.h>
#include <Functions/FunctionBinaryArithmetic.h>

namespace DB
{

template <typename A, typename B>
struct DivideFloatingOrNullImpl
{
    using ResultType = typename NumberTraits::ResultOfFloatingPointDivision<A, B>::Type;
    static const constexpr bool allow_fixed_string = false;
    static const constexpr bool allow_string_integer = false;

    template <typename Result = ResultType>
    static NO_SANITIZE_UNDEFINED Result apply(A a [[maybe_unused]], B b [[maybe_unused]])
    {
        return static_cast<Result>(a) / static_cast<Result>(b);
    }

#if USE_EMBEDDED_COMPILER
    static constexpr bool compilable = false;
#endif
};

struct NameDivideOrNull { static constexpr auto name = "divideOrNull"; };
using FunctionDivideOrNull = BinaryArithmeticOverloadResolver<DivideFloatingOrNullImpl, NameDivideOrNull>;

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
