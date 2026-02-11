#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Explicitly instantiate decimal comparison dispatch for equals.
/// This provides the executeDecimal definition that equalsHalf3.cpp suppresses
/// via COMPARISON_EXTERN_DECIMAL_TEMPLATES.
COMPARISON_INSTANTIATE_DECIMAL(EqualsOp, NameEquals)

}
