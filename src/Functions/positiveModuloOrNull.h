#pragma once

#include <Functions/FunctionBinaryArithmetic.h>

namespace DB
{

struct NamePositiveModuloOrNull
{
    static constexpr auto name = "positiveModuloOrNull";
};
using FunctionPositiveModuloOrNll = BinaryArithmeticOverloadResolver<PositiveModuloOrNullImpl, NamePositiveModuloOrNull, false>;

}
