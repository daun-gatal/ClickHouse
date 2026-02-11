#pragma once

#include <string>
#include <string_view>

namespace DB
{

enum class ExternalCommandStderrReaction : uint8_t
{
    NONE, /// Do nothing.
    LOG, /// Try to log all outputs of stderr from the external command immediately.
    LOG_FIRST, /// Try to log first 1_KiB outputs of stderr from the external command after exit.
    LOG_LAST, /// Same as above, but log last 1_KiB outputs.
    THROW /// Immediately throw exception when the external command outputs something to its stderr.
};

inline std::string_view enumToString(ExternalCommandStderrReaction reaction)
{
    switch (reaction)
    {
        case ExternalCommandStderrReaction::NONE: return "NONE";
        case ExternalCommandStderrReaction::LOG: return "LOG";
        case ExternalCommandStderrReaction::LOG_FIRST: return "LOG_FIRST";
        case ExternalCommandStderrReaction::LOG_LAST: return "LOG_LAST";
        case ExternalCommandStderrReaction::THROW: return "THROW";
    }
}

ExternalCommandStderrReaction parseExternalCommandStderrReaction(const std::string & config);

}
