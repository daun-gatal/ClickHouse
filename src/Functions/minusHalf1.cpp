#include <Functions/minus.h>

namespace DB
{

template class FunctionBinaryArithmetic<MinusImpl, NameMinus>;
template class FunctionBinaryArithmeticWithConstants<MinusImpl, NameMinus>;
template class BinaryArithmeticOverloadResolver<MinusImpl, NameMinus>;

}
