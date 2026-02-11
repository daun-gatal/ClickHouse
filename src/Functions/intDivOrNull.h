#pragma once

#include <Functions/FunctionBinaryArithmetic.h>

namespace DB
{

struct NameIntDivOrNull { static constexpr auto name = "intDivOrNull"; };
using FunctionIntDivOrNull = BinaryArithmeticOverloadResolver<DivideIntegralOrNullImpl, NameIntDivOrNull, false>;

}
