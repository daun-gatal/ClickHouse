#include <Functions/FunctionFactory.h>
#include <Functions/FunctionBinaryArithmetic.h>

namespace DB
{

struct NameModuloLegacy { static constexpr auto name = "moduloLegacy"; };
using FunctionModuloLegacy = BinaryArithmeticOverloadResolver<ModuloLegacyImpl, NameModuloLegacy, false>;

REGISTER_FUNCTION(ModuloLegacy)
{
    factory.registerFunction<FunctionModuloLegacy>();
}

}
