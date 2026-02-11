#include <Functions/intDiv.h>

namespace DB
{

/// Suppress division_by_nullable=true variants — provided by intDivHalf2.cpp.
extern template class FunctionBinaryArithmetic<DivideIntegralImpl, NameIntDiv, false, true, true>;
extern template class FunctionBinaryArithmeticWithConstants<DivideIntegralImpl, NameIntDiv, false, true, true>;

/// Explicitly instantiate the full intDiv arithmetic classes.
/// This provides all member function definitions that intDiv.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<DivideIntegralImpl, NameIntDiv, false>;
template class FunctionBinaryArithmeticWithConstants<DivideIntegralImpl, NameIntDiv, false>;
template class BinaryArithmeticOverloadResolver<DivideIntegralImpl, NameIntDiv, false>;

}
