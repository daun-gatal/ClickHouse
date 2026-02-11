#include <Functions/moduloOrZero.h>

namespace DB
{

template class FunctionBinaryArithmetic<ModuloOrZeroImpl, NameModuloOrZero>;
template class FunctionBinaryArithmeticWithConstants<ModuloOrZeroImpl, NameModuloOrZero>;
template class BinaryArithmeticOverloadResolver<ModuloOrZeroImpl, NameModuloOrZero>;

}
