#include <Functions/divide.h>

namespace DB
{

/// Explicitly instantiate the division_by_nullable=true variants of divide.
template class FunctionBinaryArithmetic<DivideFloatingImpl, NameDivide, true, true, true>;
template class FunctionBinaryArithmeticWithConstants<DivideFloatingImpl, NameDivide, true, true, true>;

}
