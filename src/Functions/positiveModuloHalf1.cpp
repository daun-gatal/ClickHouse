#include <Functions/positiveModulo.h>

namespace DB
{

template class FunctionBinaryArithmetic<PositiveModuloImpl, NamePositiveModulo, false>;
template class FunctionBinaryArithmeticWithConstants<PositiveModuloImpl, NamePositiveModulo, false>;
template class BinaryArithmeticOverloadResolver<PositiveModuloImpl, NamePositiveModulo, false>;

}
