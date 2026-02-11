#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Suppress numeric dispatch — provided by equalsNumHalf1.cpp / equalsNumHalf2.cpp.
COMPARISON_EXTERN_NUMERIC_TEMPLATES(EqualsOp, NameEquals)

/// Explicitly instantiate non-numeric member functions for equals.
/// This provides all member function definitions that equals.cpp suppresses
/// via COMPARISON_EXTERN_NON_NUMERIC_TEMPLATES.
COMPARISON_INSTANTIATE_NON_NUMERIC(EqualsOp, NameEquals)

}
