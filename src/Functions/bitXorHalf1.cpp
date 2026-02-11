#include <Functions/bitXor.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitXorImpl, NameBitXor, true, false>;
template class FunctionBinaryArithmeticWithConstants<BitXorImpl, NameBitXor, true, false>;
template class BinaryArithmeticOverloadResolver<BitXorImpl, NameBitXor, true, false>;

}
