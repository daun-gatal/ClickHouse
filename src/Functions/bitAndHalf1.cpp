#include <Functions/bitAnd.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitAndImpl, NameBitAnd, true, false>;
template class FunctionBinaryArithmeticWithConstants<BitAndImpl, NameBitAnd, true, false>;
template class BinaryArithmeticOverloadResolver<BitAndImpl, NameBitAnd, true, false>;

}
