#include <Functions/array/arrayDistance.h>

namespace DB
{

/// These functions are used by TupleOrArrayFunction in vectorFunctions.cpp
FunctionPtr createFunctionArrayL1Distance(ContextPtr context_) { return FunctionArrayDistance<L1Distance>::create(context_); }
FunctionPtr createFunctionArrayLinfDistance(ContextPtr context_) { return FunctionArrayDistance<LinfDistance>::create(context_); }

}
