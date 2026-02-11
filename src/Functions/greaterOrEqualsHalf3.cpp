#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Suppress numeric dispatch — provided by greaterOrEqualsNumHalf1.cpp / greaterOrEqualsNumHalf2.cpp.
COMPARISON_EXTERN_NUMERIC_TEMPLATES(GreaterOrEqualsOp, NameGreaterOrEquals)
/// Suppress decimal dispatch — provided by greaterOrEqualsHalf4.cpp.
COMPARISON_EXTERN_DECIMAL_TEMPLATES(GreaterOrEqualsOp, NameGreaterOrEquals)

/// Explicitly instantiate non-numeric member functions for greaterOrEquals.
/// This provides all member function definitions that greaterOrEquals.cpp suppresses
/// via COMPARISON_EXTERN_NON_NUMERIC_TEMPLATES.
COMPARISON_INSTANTIATE_NON_NUMERIC(GreaterOrEqualsOp, NameGreaterOrEquals)

}
