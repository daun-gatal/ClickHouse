#include <Functions/positiveModulo.h>

namespace DB
{

/// Suppress division_by_nullable=true variants — provided by positiveModuloHalf2.cpp.
extern template class FunctionBinaryArithmetic<PositiveModuloImpl, NamePositiveModulo, false, true, true>;
extern template class FunctionBinaryArithmeticWithConstants<PositiveModuloImpl, NamePositiveModulo, false, true, true>;

template class FunctionBinaryArithmetic<PositiveModuloImpl, NamePositiveModulo, false>;
template class FunctionBinaryArithmeticWithConstants<PositiveModuloImpl, NamePositiveModulo, false>;
template class BinaryArithmeticOverloadResolver<PositiveModuloImpl, NamePositiveModulo, false>;

}
