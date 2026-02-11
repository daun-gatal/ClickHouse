#include <Functions/modulo.h>

namespace DB
{

/// Suppress division_by_nullable=true variants — provided by moduloHalf2.cpp.
extern template class FunctionBinaryArithmetic<ModuloImpl, NameModulo, false, true, true>;
extern template class FunctionBinaryArithmeticWithConstants<ModuloImpl, NameModulo, false, true, true>;

template class FunctionBinaryArithmetic<ModuloImpl, NameModulo, false>;
template class FunctionBinaryArithmeticWithConstants<ModuloImpl, NameModulo, false>;
template class BinaryArithmeticOverloadResolver<ModuloImpl, NameModulo, false>;

}
