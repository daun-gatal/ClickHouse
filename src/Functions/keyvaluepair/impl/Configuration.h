#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace DB
{
namespace extractKV
{
struct ConfigurationFactory;

class Configuration
{
public:
    enum class UnexpectedQuotingCharacterStrategy
    {
        INVALID,
        ACCEPT,
        PROMOTE
    };

    static std::optional<UnexpectedQuotingCharacterStrategy> parseUnexpectedQuotingCharacterStrategy(std::string_view str)
    {
        /// Case-insensitive comparison
        auto upper = [](std::string_view s) -> std::string
        {
            std::string result(s);
            for (auto & c : result)
                if (c >= 'a' && c <= 'z')
                    c -= 32;
            return result;
        };
        auto upper_str = upper(str);
        if (upper_str == "INVALID") return UnexpectedQuotingCharacterStrategy::INVALID;
        if (upper_str == "ACCEPT") return UnexpectedQuotingCharacterStrategy::ACCEPT;
        if (upper_str == "PROMOTE") return UnexpectedQuotingCharacterStrategy::PROMOTE;
        return std::nullopt;
    }

    const char key_value_delimiter;
    const char quoting_character;
    const std::vector<char> pair_delimiters;
    const UnexpectedQuotingCharacterStrategy unexpected_quoting_character_strategy;

private:
    friend struct ConfigurationFactory;

    Configuration(
        char key_value_delimiter_,
        char quoting_character_,
        std::vector<char> pair_delimiters_,
        UnexpectedQuotingCharacterStrategy unexpected_quoting_character_strategy_
    );
};

/*
 * Validates (business logic) and creates Configurations for key-value-pair extraction.
 * */
struct ConfigurationFactory
{
public:
    static Configuration createWithoutEscaping(char key_value_delimiter, char quoting_character, std::vector<char> pair_delimiters, Configuration::UnexpectedQuotingCharacterStrategy unexpected_quoting_character_strategy);

    static Configuration createWithEscaping(char key_value_delimiter, char quoting_character, std::vector<char> pair_delimiters, Configuration::UnexpectedQuotingCharacterStrategy unexpected_quoting_character_strategy);

private:
    static void validate(char key_value_delimiter, char quoting_character, std::vector<char> pair_delimiters);

    static constexpr auto MAX_NUMBER_OF_PAIR_DELIMITERS = 8u;
};
}

}
