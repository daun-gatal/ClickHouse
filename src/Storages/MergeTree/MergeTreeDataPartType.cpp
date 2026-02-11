#include <base/types.h>
#include <Common/Exception.h>

#include <Storages/MergeTree/MergeTreeDataPartType.h>

namespace DB
{

namespace ErrorCodes
{
extern const int BAD_ARGUMENTS;
}

String MergeTreeDataPartType::toString() const
{
    switch (value)
    {
        case Value::Wide: return "Wide";
        case Value::Compact: return "Compact";
        case Value::Unknown: return "Unknown";
    }
}

void MergeTreeDataPartType::fromString(const String & str)
{
    if (str == "Wide")
        value = Value::Wide;
    else if (str == "Compact")
        value = Value::Compact;
    else
        throw Exception(ErrorCodes::BAD_ARGUMENTS, "Unexpected string {} for enum MergeTreeDataPartType::Value", str);
}

String MergeTreeDataPartStorageType::toString() const
{
    switch (value)
    {
        case Value::Full: return "Full";
        case Value::Packed: return "Packed";
        case Value::Unknown: return "Unknown";
    }
}

void MergeTreeDataPartStorageType::fromString(const String & str)
{
    if (str == "Full")
        value = Value::Full;
    else if (str == "Packed")
        value = Value::Packed;
    else
        throw Exception(ErrorCodes::BAD_ARGUMENTS, "Unexpected string {} for enum MergeTreeDataPartStorageType::Value", str);
}

}
