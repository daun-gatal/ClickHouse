#include <base/types.h>
#include <Common/Exception.h>

#include <Storages/MergeTree/MergeTreeDataPartType.h>

namespace DB
{

namespace ErrorCodes
{
extern const int BAD_ARGUMENTS;
}

template <typename E>
requires std::is_enum_v<E>
static E parseEnum(const String & str)
{
    if constexpr (std::is_same_v<E, MergeTreeDataPartType::Value>)
    {
        if (str == "Wide") return E::Wide;
        if (str == "Compact") return E::Compact;
    }
    else if constexpr (std::is_same_v<E, MergeTreeDataPartStorageType::Value>)
    {
        if (str == "Full") return E::Full;
        if (str == "Packed") return E::Packed;
    }

    throw DB::Exception(ErrorCodes::BAD_ARGUMENTS, "Unexpected string {} for enum", str);
}

String MergeTreeDataPartType::toString() const
{
    return String(DB::enumToString(value));
}

void MergeTreeDataPartType::fromString(const String & str)
{
    value = parseEnum<Value>(str);
}

String MergeTreeDataPartStorageType::toString() const
{
    return String(DB::enumToString(value));
}

void MergeTreeDataPartStorageType::fromString(const String & str)
{
    value = parseEnum<Value>(str);
}

}
