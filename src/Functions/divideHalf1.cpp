#include <Functions/divide.h>

namespace DB
{

/// Explicitly instantiate the full divide arithmetic classes.
/// This provides all member function definitions that divide.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<DivideFloatingImpl, NameDivide>;
template class FunctionBinaryArithmeticWithConstants<DivideFloatingImpl, NameDivide>;
template class BinaryArithmeticOverloadResolver<DivideFloatingImpl, NameDivide>;

}
