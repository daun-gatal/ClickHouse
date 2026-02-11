#include <Functions/moduloOrNull.h>

namespace DB
{

template class FunctionBinaryArithmetic<ModuloOrNullImpl, NameModuloOrNull, false>;
template class FunctionBinaryArithmeticWithConstants<ModuloOrNullImpl, NameModuloOrNull, false>;
template class BinaryArithmeticOverloadResolver<ModuloOrNullImpl, NameModuloOrNull, false>;

}
