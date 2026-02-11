#include <Functions/bitRotateLeft.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitRotateLeftImpl, NameBitRotateLeft, true, false>;
template class FunctionBinaryArithmeticWithConstants<BitRotateLeftImpl, NameBitRotateLeft, true, false>;
template class BinaryArithmeticOverloadResolver<BitRotateLeftImpl, NameBitRotateLeft, true, false>;

}
