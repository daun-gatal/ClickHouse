#include <Functions/array/arrayDistance.h>

namespace DB
{

/// These functions are used by TupleOrArrayFunction in vectorFunctions.cpp
FunctionPtr createFunctionArrayL2Distance(ContextPtr context_) { return FunctionArrayDistance<L2Distance>::create(context_); }
FunctionPtr createFunctionArrayL2SquaredDistance(ContextPtr context_) { return FunctionArrayDistance<L2SquaredDistance>::create(context_); }

}
