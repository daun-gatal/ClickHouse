#include <Functions/positiveModulo.h>

namespace DB
{

/// Explicitly instantiate the division_by_nullable=true variants of positiveModulo.
template class FunctionBinaryArithmetic<PositiveModuloImpl, NamePositiveModulo, false, true, true>;
template class FunctionBinaryArithmeticWithConstants<PositiveModuloImpl, NamePositiveModulo, false, true, true>;

}
