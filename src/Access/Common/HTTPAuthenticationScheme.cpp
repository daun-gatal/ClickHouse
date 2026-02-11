#include <Access/Common/HTTPAuthenticationScheme.h>

#include <base/types.h>
#include <Poco/String.h>
#include <Common/Exception.h>

namespace DB
{
namespace ErrorCodes
{
    extern const int BAD_ARGUMENTS;
}


String enumToString(HTTPAuthenticationScheme scheme)
{
    switch (scheme)
    {
        case HTTPAuthenticationScheme::BASIC: return "BASIC";
    }
    return "Unknown HTTP authentication scheme";
}

HTTPAuthenticationScheme parseHTTPAuthenticationScheme(const String & scheme_str)
{
    String upper = Poco::toUpper(scheme_str);
    if (upper == "BASIC")
        return HTTPAuthenticationScheme::BASIC;
    throw Exception(ErrorCodes::BAD_ARGUMENTS, "Unknown HTTP authentication scheme: {}. Possible value is 'BASIC'", scheme_str);
}

}
