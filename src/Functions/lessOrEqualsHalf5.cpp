#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Explicitly instantiate decimal comparison dispatch Part2 for lessOrEquals.
/// Handles Decimal256, DateTime64, Time64 type combinations.
COMPARISON_INSTANTIATE_DECIMAL_PART2(LessOrEqualsOp, NameLessOrEquals)

}
