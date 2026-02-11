#pragma once

#include <base/types.h>
#include <string_view>

namespace DB
{
/// Algorithm of Merge.
enum class MergeAlgorithm : uint8_t
{
    Undecided, /// Not running yet
    Horizontal, /// per-row merge of all columns
    Vertical /// per-row merge of PK and secondary indices columns, per-column gather for non-PK columns
};

inline std::string_view enumToString(MergeAlgorithm value)
{
    switch (value)
    {
        case MergeAlgorithm::Undecided: return "Undecided";
        case MergeAlgorithm::Horizontal: return "Horizontal";
        case MergeAlgorithm::Vertical: return "Vertical";
    }
    return "Unknown";
}

}
