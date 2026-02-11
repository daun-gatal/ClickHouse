#include <Functions/least.h>

namespace DB
{

/// Explicitly instantiate the full least arithmetic class.
/// This provides all member function definitions that least.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<LeastImpl, NameLeast>;

}
