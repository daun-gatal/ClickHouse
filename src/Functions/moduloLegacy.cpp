#include <Functions/moduloLegacy.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<ModuloLegacyImpl, NameModuloLegacy, false>;
extern template class FunctionBinaryArithmeticWithConstants<ModuloLegacyImpl, NameModuloLegacy, false>;
extern template class BinaryArithmeticOverloadResolver<ModuloLegacyImpl, NameModuloLegacy, false>;

REGISTER_FUNCTION(ModuloLegacy)
{
    factory.registerFunction<FunctionModuloLegacy>();
}

}
