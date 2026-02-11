#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Suppress numeric dispatch — provided by lessNumHalf1.cpp / lessNumHalf2.cpp.
COMPARISON_EXTERN_NUMERIC_TEMPLATES(LessOp, NameLess)

/// Explicitly instantiate non-numeric member functions for less.
/// This provides all member function definitions that less.cpp suppresses
/// via COMPARISON_EXTERN_NON_NUMERIC_TEMPLATES.
COMPARISON_INSTANTIATE_NON_NUMERIC(LessOp, NameLess)

}
