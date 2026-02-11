#include <Functions/FunctionsComparison.h>

namespace DB
{

/// Explicitly instantiate decimal comparison dispatch Part1 for greater.
/// Handles Decimal32, Decimal64, Decimal128 type combinations.
COMPARISON_INSTANTIATE_DECIMAL_PART1(GreaterOp, NameGreater)

}
