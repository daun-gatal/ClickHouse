#include <Functions/bitBoolMaskAnd.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitBoolMaskAndImpl, NameBitBoolMaskAnd>;
template class FunctionBinaryArithmeticWithConstants<BitBoolMaskAndImpl, NameBitBoolMaskAnd>;
template class BinaryArithmeticOverloadResolver<BitBoolMaskAndImpl, NameBitBoolMaskAnd>;

}
