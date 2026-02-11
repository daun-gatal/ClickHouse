#pragma once

#include <base/types.h>

namespace DB
{

enum class HTTPAuthenticationScheme
{
    BASIC,
};


String enumToString(HTTPAuthenticationScheme scheme);
HTTPAuthenticationScheme parseHTTPAuthenticationScheme(const String & scheme_str);
}
