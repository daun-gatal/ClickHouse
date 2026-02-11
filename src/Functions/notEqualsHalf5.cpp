#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Explicitly instantiate decimal comparison dispatch Part2 for notEquals.
/// Handles Decimal256, DateTime64, Time64 type combinations.
COMPARISON_INSTANTIATE_DECIMAL_PART2(NotEqualsOp, NameNotEquals)

}
