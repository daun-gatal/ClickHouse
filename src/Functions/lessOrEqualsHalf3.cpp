#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Suppress numeric dispatch — provided by lessOrEqualsNumHalf1.cpp / lessOrEqualsNumHalf2.cpp.
COMPARISON_EXTERN_NUMERIC_TEMPLATES(LessOrEqualsOp, NameLessOrEquals)
/// Suppress decimal dispatch — provided by lessOrEqualsHalf4.cpp.
COMPARISON_EXTERN_DECIMAL_TEMPLATES(LessOrEqualsOp, NameLessOrEquals)

/// Explicitly instantiate non-numeric member functions for lessOrEquals.
/// This provides all member function definitions that lessOrEquals.cpp suppresses
/// via COMPARISON_EXTERN_NON_NUMERIC_TEMPLATES.
COMPARISON_INSTANTIATE_NON_NUMERIC(LessOrEqualsOp, NameLessOrEquals)

}
