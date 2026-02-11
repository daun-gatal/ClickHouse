#pragma once

#include <Functions/FunctionBinaryArithmetic.h>

namespace DB
{

struct NamePositiveModulo
{
    static constexpr auto name = "positiveModulo";
};
using FunctionPositiveModulo = BinaryArithmeticOverloadResolver<PositiveModuloImpl, NamePositiveModulo, false>;

}
