#include <Functions/bitBoolMaskOr.h>
#include <Functions/FunctionFactory.h>

namespace DB
{

extern template class FunctionBinaryArithmetic<BitBoolMaskOrImpl, NameBitBoolMaskOr>;
extern template class FunctionBinaryArithmeticWithConstants<BitBoolMaskOrImpl, NameBitBoolMaskOr>;
extern template class BinaryArithmeticOverloadResolver<BitBoolMaskOrImpl, NameBitBoolMaskOr>;

REGISTER_FUNCTION(BitBoolMaskOr)
{
    factory.registerFunction<FunctionBitBoolMaskOr>();
}

}
