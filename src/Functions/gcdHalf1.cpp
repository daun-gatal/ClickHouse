#include <Functions/gcd.h>

namespace DB
{

/// Explicitly instantiate the full gcd arithmetic classes.
/// This provides all member function definitions that gcd.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<GCDImpl, NameGCD, false, false>;
template class FunctionBinaryArithmeticWithConstants<GCDImpl, NameGCD, false, false>;
template class BinaryArithmeticOverloadResolver<GCDImpl, NameGCD, false, false>;

}
