#include <Functions/intDiv.h>

namespace DB
{

/// Explicitly instantiate the division_by_nullable=true variants of intDiv.
template class FunctionBinaryArithmetic<DivideIntegralImpl, NameIntDiv, false, true, true>;
template class FunctionBinaryArithmeticWithConstants<DivideIntegralImpl, NameIntDiv, false, true, true>;

}
