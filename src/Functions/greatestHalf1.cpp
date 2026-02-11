#include <Functions/greatest.h>

namespace DB
{

/// Explicitly instantiate the full greatest arithmetic class.
/// This provides all member function definitions that greatest.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<GreatestImpl, NameGreatest>;

}
