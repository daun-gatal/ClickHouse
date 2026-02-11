#include <Functions/modulo.h>

namespace DB
{

template class FunctionBinaryArithmetic<ModuloImpl, NameModulo, false>;
template class FunctionBinaryArithmeticWithConstants<ModuloImpl, NameModulo, false>;
template class BinaryArithmeticOverloadResolver<ModuloImpl, NameModulo, false>;

}
