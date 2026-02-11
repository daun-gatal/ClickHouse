#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Explicitly instantiate decimal comparison dispatch for less.
/// This provides the executeDecimal definition that lessHalf3.cpp suppresses
/// via COMPARISON_EXTERN_DECIMAL_TEMPLATES.
COMPARISON_INSTANTIATE_DECIMAL(LessOp, NameLess)

}
