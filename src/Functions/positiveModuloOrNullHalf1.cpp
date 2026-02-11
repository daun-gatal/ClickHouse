#include <Functions/positiveModuloOrNull.h>

namespace DB
{

template class FunctionBinaryArithmetic<PositiveModuloOrNullImpl, NamePositiveModuloOrNull, false>;
template class FunctionBinaryArithmeticWithConstants<PositiveModuloOrNullImpl, NamePositiveModuloOrNull, false>;
template class BinaryArithmeticOverloadResolver<PositiveModuloOrNullImpl, NamePositiveModuloOrNull, false>;

}
