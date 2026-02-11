#include <Functions/plus.h>

namespace DB
{

/// Explicitly instantiate the full plus arithmetic classes.
/// This provides all member function definitions that plus.cpp suppresses via extern template class.
template class FunctionBinaryArithmetic<PlusImpl, NamePlus>;
template class FunctionBinaryArithmeticWithConstants<PlusImpl, NamePlus>;
template class BinaryArithmeticOverloadResolver<PlusImpl, NamePlus>;

}
