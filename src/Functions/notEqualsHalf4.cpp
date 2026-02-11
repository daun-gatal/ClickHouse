#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Explicitly instantiate decimal comparison dispatch for notEquals.
/// This provides the executeDecimal definition that notEqualsHalf3.cpp suppresses
/// via COMPARISON_EXTERN_DECIMAL_TEMPLATES.
COMPARISON_INSTANTIATE_DECIMAL(NotEqualsOp, NameNotEquals)

}
