#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Suppress numeric dispatch — provided by notEqualsNumHalf1.cpp / notEqualsNumHalf2.cpp.
COMPARISON_EXTERN_NUMERIC_TEMPLATES(NotEqualsOp, NameNotEquals)
/// Suppress decimal dispatch — provided by notEqualsHalf4.cpp.
COMPARISON_EXTERN_DECIMAL_TEMPLATES(NotEqualsOp, NameNotEquals)

/// Explicitly instantiate non-numeric member functions for notEquals.
/// This provides all member function definitions that notEquals.cpp suppresses
/// via COMPARISON_EXTERN_NON_NUMERIC_TEMPLATES.
COMPARISON_INSTANTIATE_NON_NUMERIC(NotEqualsOp, NameNotEquals)

}
