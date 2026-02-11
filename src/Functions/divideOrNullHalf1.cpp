#include <Functions/divideOrNull.h>

namespace DB
{

/// Explicitly instantiate the full divideOrNull arithmetic classes.
/// This provides all member function definitions that divideOrNull.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<DivideFloatingOrNullImpl, NameDivideOrNull>;
template class FunctionBinaryArithmeticWithConstants<DivideFloatingOrNullImpl, NameDivideOrNull>;
template class BinaryArithmeticOverloadResolver<DivideFloatingOrNullImpl, NameDivideOrNull>;

}
