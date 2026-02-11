#pragma once

#include <Functions/FunctionBinaryArithmetic.h>

namespace DB
{

struct NameModuloLegacy { static constexpr auto name = "moduloLegacy"; };
using FunctionModuloLegacy = BinaryArithmeticOverloadResolver<ModuloLegacyImpl, NameModuloLegacy, false>;

}
