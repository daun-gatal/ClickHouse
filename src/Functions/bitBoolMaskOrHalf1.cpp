#include <Functions/bitBoolMaskOr.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitBoolMaskOrImpl, NameBitBoolMaskOr>;
template class FunctionBinaryArithmeticWithConstants<BitBoolMaskOrImpl, NameBitBoolMaskOr>;
template class BinaryArithmeticOverloadResolver<BitBoolMaskOrImpl, NameBitBoolMaskOr>;

}
