#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Suppress numeric dispatch — provided by greaterNumHalf1.cpp / greaterNumHalf2.cpp.
COMPARISON_EXTERN_NUMERIC_TEMPLATES(GreaterOp, NameGreater)

/// Explicitly instantiate non-numeric member functions for greater.
/// This provides all member function definitions that greater.cpp suppresses
/// via COMPARISON_EXTERN_NON_NUMERIC_TEMPLATES.
COMPARISON_INSTANTIATE_NON_NUMERIC(GreaterOp, NameGreater)

}
