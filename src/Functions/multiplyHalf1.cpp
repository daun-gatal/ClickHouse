#include <Functions/multiply.h>

namespace DB
{

template class FunctionBinaryArithmetic<MultiplyImpl, NameMultiply>;
template class FunctionBinaryArithmeticWithConstants<MultiplyImpl, NameMultiply>;
template class BinaryArithmeticOverloadResolver<MultiplyImpl, NameMultiply>;

}
