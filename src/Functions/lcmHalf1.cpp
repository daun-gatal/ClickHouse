#include <Functions/lcm.h>

namespace DB
{

/// Explicitly instantiate the full lcm arithmetic classes.
/// This provides all member function definitions that lcm.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<LCMImpl, NameLCM, false, false>;
template class FunctionBinaryArithmeticWithConstants<LCMImpl, NameLCM, false, false>;
template class BinaryArithmeticOverloadResolver<LCMImpl, NameLCM, false, false>;

}
