#include <Functions/array/arrayDistance.h>

namespace DB
{

/// This function is used by TupleOrArrayFunction in vectorFunctions.cpp
FunctionPtr createFunctionArrayCosineDistance(ContextPtr context_) { return FunctionArrayDistance<CosineDistance>::create(context_); }

}
