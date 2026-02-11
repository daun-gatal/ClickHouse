#include <Common/Exception.h>
#include <Common/ShellCommandSettings.h>

#include <Poco/String.h>

namespace DB
{

namespace ErrorCodes
{
    extern const int BAD_ARGUMENTS;
}

ExternalCommandStderrReaction parseExternalCommandStderrReaction(const std::string & config)
{
    auto upper = Poco::toUpper(config);
    if (upper == "NONE") return ExternalCommandStderrReaction::NONE;
    if (upper == "LOG") return ExternalCommandStderrReaction::LOG;
    if (upper == "LOG_FIRST") return ExternalCommandStderrReaction::LOG_FIRST;
    if (upper == "LOG_LAST") return ExternalCommandStderrReaction::LOG_LAST;
    if (upper == "THROW") return ExternalCommandStderrReaction::THROW;

    throw Exception(
        ErrorCodes::BAD_ARGUMENTS,
        "Unknown stderr_reaction: {}. Possible values are 'none', 'log', 'log_first', 'log_last' and 'throw'",
        config);
}

}
