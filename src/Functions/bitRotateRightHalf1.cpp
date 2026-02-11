#include <Functions/bitRotateRight.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitRotateRightImpl, NameBitRotateRight, true, false>;
template class FunctionBinaryArithmeticWithConstants<BitRotateRightImpl, NameBitRotateRight, true, false>;
template class BinaryArithmeticOverloadResolver<BitRotateRightImpl, NameBitRotateRight, true, false>;

}
