#include <Functions/bitOr.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitOrImpl, NameBitOr, true, false>;
template class FunctionBinaryArithmeticWithConstants<BitOrImpl, NameBitOr, true, false>;
template class BinaryArithmeticOverloadResolver<BitOrImpl, NameBitOr, true, false>;

}
