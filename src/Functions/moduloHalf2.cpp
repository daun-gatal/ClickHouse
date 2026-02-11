#include <Functions/modulo.h>

namespace DB
{

/// Explicitly instantiate the division_by_nullable=true variants of modulo.
/// These are triggered by BinaryArithmeticOverloadResolver::buildImpl but
/// suppressed in moduloHalf1.cpp via extern template class.
template class FunctionBinaryArithmetic<ModuloImpl, NameModulo, false, true, true>;
template class FunctionBinaryArithmeticWithConstants<ModuloImpl, NameModulo, false, true, true>;

}
