#include <Functions/divide.h>

namespace DB
{

/// Suppress division_by_nullable=true variants — provided by divideHalf2.cpp.
extern template class FunctionBinaryArithmetic<DivideFloatingImpl, NameDivide, true, true, true>;
extern template class FunctionBinaryArithmeticWithConstants<DivideFloatingImpl, NameDivide, true, true, true>;

/// Explicitly instantiate the full divide arithmetic classes.
/// This provides all member function definitions that divide.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<DivideFloatingImpl, NameDivide>;
template class FunctionBinaryArithmeticWithConstants<DivideFloatingImpl, NameDivide>;
template class BinaryArithmeticOverloadResolver<DivideFloatingImpl, NameDivide>;

}
