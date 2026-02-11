#include <Functions/midpoint.h>

namespace DB
{

/// Explicitly instantiate the full midpoint arithmetic class.
/// This provides all member function definitions that midpoint.cpp / avg2.cpp suppress via extern template class.
template class FunctionBinaryArithmetic<MidpointImpl, NameMidpoint>;

}
