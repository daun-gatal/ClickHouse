#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Explicitly instantiate decimal comparison dispatch for lessOrEquals.
/// This provides the executeDecimal definition that lessOrEqualsHalf3.cpp suppresses
/// via COMPARISON_EXTERN_DECIMAL_TEMPLATES.
COMPARISON_INSTANTIATE_DECIMAL(LessOrEqualsOp, NameLessOrEquals)

}
