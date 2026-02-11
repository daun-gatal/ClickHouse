#include <Functions/bitBoolMaskAnd.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitBoolMaskAndImpl, NameBitBoolMaskAnd>;
extern template class FunctionBinaryArithmeticWithConstants<BitBoolMaskAndImpl, NameBitBoolMaskAnd>;
extern template class BinaryArithmeticOverloadResolver<BitBoolMaskAndImpl, NameBitBoolMaskAnd>;

REGISTER_FUNCTION(BitBoolMaskAnd)
{
    factory.registerFunction<FunctionBitBoolMaskAnd>();
}

}
