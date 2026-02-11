#include <Functions/intDivOrZero.h>

namespace DB
{

/// Explicitly instantiate the full intDivOrZero arithmetic classes.
/// This provides all member function definitions that intDivOrZero.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<DivideIntegralOrZeroImpl, NameIntDivOrZero>;
template class FunctionBinaryArithmeticWithConstants<DivideIntegralOrZeroImpl, NameIntDivOrZero>;
template class BinaryArithmeticOverloadResolver<DivideIntegralOrZeroImpl, NameIntDivOrZero>;

}
