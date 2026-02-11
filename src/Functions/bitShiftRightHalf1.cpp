#include <Functions/bitShiftRight.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitShiftRightImpl, NameBitShiftRight, true, false>;
template class FunctionBinaryArithmeticWithConstants<BitShiftRightImpl, NameBitShiftRight, true, false>;
template class BinaryArithmeticOverloadResolver<BitShiftRightImpl, NameBitShiftRight, true, false>;

}
