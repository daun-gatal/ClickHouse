#pragma once

#include <string_view>


namespace DB
{

namespace extractKV
{

class StateHandler
{
public:
    enum State
    {
        // Skip characters until it finds a valid first key character. Might jump to READING_KEY, READING_QUOTED_KEY or END.
        WAITING_KEY,
        // Tries to read a key. Might jump to WAITING_KEY, WAITING_VALUE or END.
        READING_KEY,
        // Tries to read a quoted key. Might jump to WAITING_KEY, READING_KV_DELIMITER or END.
        READING_QUOTED_KEY,
        // Tries to read the key value pair delimiter. Might jump to WAITING_KEY, WAITING_VALUE or END.
        READING_KV_DELIMITER,
        // Skip characters until it finds a valid first value character. Might jump to READING_QUOTED_VALUE or READING_VALUE.
        WAITING_VALUE,
        // Tries to read a value. Jumps to FLUSH_PAIR.
        READING_VALUE,
        // Tries to read a quoted value. Might jump to FLUSH_PAIR or END.
        READING_QUOTED_VALUE,
        // In this state, both key and value have already been collected and should be flushed. Might jump to WAITING_KEY or END.
        FLUSH_PAIR,
        // The `READING_QUOTED_VALUE` will assert the closing quoting character is found and then flush the pair. In this case, we should not
        // move from `FLUSH_PAIR` directly to `WAITING_FOR_KEY` because a pair delimiter has not been found. Might jump to WAITING_FOR_PAIR_DELIMITER or END
        FLUSH_PAIR_AFTER_QUOTED_VALUE,
        // Might jump to WAITING_KEY or END.
        WAITING_PAIR_DELIMITER,
        END
    };

    static constexpr std::string_view stateToString(State state)
    {
        switch (state)
        {
            case WAITING_KEY: return "WAITING_KEY";
            case READING_KEY: return "READING_KEY";
            case READING_QUOTED_KEY: return "READING_QUOTED_KEY";
            case READING_KV_DELIMITER: return "READING_KV_DELIMITER";
            case WAITING_VALUE: return "WAITING_VALUE";
            case READING_VALUE: return "READING_VALUE";
            case READING_QUOTED_VALUE: return "READING_QUOTED_VALUE";
            case FLUSH_PAIR: return "FLUSH_PAIR";
            case FLUSH_PAIR_AFTER_QUOTED_VALUE: return "FLUSH_PAIR_AFTER_QUOTED_VALUE";
            case WAITING_PAIR_DELIMITER: return "WAITING_PAIR_DELIMITER";
            case END: return "END";
        }
        return "UNKNOWN";
    }

    struct NextState
    {
        std::size_t position_in_string;
        State state;
    };

    StateHandler() = default;
    StateHandler(const StateHandler &) = default;

    virtual ~StateHandler() = default;
};

}

}
