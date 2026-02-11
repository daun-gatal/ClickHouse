#include <Functions/moduloLegacy.h>

namespace DB
{

template class FunctionBinaryArithmetic<ModuloLegacyImpl, NameModuloLegacy, false>;
template class FunctionBinaryArithmeticWithConstants<ModuloLegacyImpl, NameModuloLegacy, false>;
template class BinaryArithmeticOverloadResolver<ModuloLegacyImpl, NameModuloLegacy, false>;

}
