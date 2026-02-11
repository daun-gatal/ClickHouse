#include <Functions/bitTest.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitTestImpl, NameBitTest, true, false>;
template class FunctionBinaryArithmeticWithConstants<BitTestImpl, NameBitTest, true, false>;
template class BinaryArithmeticOverloadResolver<BitTestImpl, NameBitTest, true, false>;

}
