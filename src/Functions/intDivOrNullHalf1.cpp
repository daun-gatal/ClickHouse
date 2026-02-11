#include <Functions/intDivOrNull.h>

namespace DB
{

/// Explicitly instantiate the full intDivOrNull arithmetic classes.
/// This provides all member function definitions that intDivOrNull.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<DivideIntegralOrNullImpl, NameIntDivOrNull, false>;
template class FunctionBinaryArithmeticWithConstants<DivideIntegralOrNullImpl, NameIntDivOrNull, false>;
template class BinaryArithmeticOverloadResolver<DivideIntegralOrNullImpl, NameIntDivOrNull, false>;

}
