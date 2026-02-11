#include <Functions/bitShiftLeft.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitShiftLeftImpl, NameBitShiftLeft, true, false>;
template class FunctionBinaryArithmeticWithConstants<BitShiftLeftImpl, NameBitShiftLeft, true, false>;
template class BinaryArithmeticOverloadResolver<BitShiftLeftImpl, NameBitShiftLeft, true, false>;

}
