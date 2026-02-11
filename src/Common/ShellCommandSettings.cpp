#include <Common/Exception.h>
#include <Common/ShellCommandSettings.h>

#include <Poco/String.h>

#include <optional>

namespace DB
{

namespace ErrorCodes
{
    extern const int BAD_ARGUMENTS;
}

static std::optional<ExternalCommandStderrReaction> externalCommandStderrReactionFromString(const std::string & str)
{
    if (str == "NONE") return ExternalCommandStderrReaction::NONE;
    if (str == "LOG") return ExternalCommandStderrReaction::LOG;
    if (str == "LOG_FIRST") return ExternalCommandStderrReaction::LOG_FIRST;
    if (str == "LOG_LAST") return ExternalCommandStderrReaction::LOG_LAST;
    if (str == "THROW") return ExternalCommandStderrReaction::THROW;
    return std::nullopt;
}

ExternalCommandStderrReaction parseExternalCommandStderrReaction(const std::string & config)
{
    auto reaction = externalCommandStderrReactionFromString(Poco::toUpper(config));
    if (!reaction)
        throw Exception(
            ErrorCodes::BAD_ARGUMENTS,
            "Unknown stderr_reaction: {}. Possible values are 'none', 'log', 'log_first', 'log_last' and 'throw'",
            config);

    return *reaction;
}

}
