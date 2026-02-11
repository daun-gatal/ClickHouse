#include <Storages/MergeTree/MergeType.h>
#include <Common/Exception.h>

namespace DB
{

namespace ErrorCodes
{
    extern const int NOT_IMPLEMENTED;
}

MergeType checkAndGetMergeType(std::underlying_type_t<MergeType> merge_type)
{
    switch (static_cast<MergeType>(merge_type))
    {
        case MergeType::Regular:
        case MergeType::TTLDelete:
        case MergeType::TTLRecompress:
        case MergeType::TTLDrop:
            return static_cast<MergeType>(merge_type);
    }

    throw Exception(ErrorCodes::NOT_IMPLEMENTED, "Unknown MergeType {}", static_cast<UInt64>(merge_type));
}

bool isTTLMergeType(MergeType merge_type)
{
    return merge_type == MergeType::TTLDelete || merge_type == MergeType::TTLRecompress || merge_type == MergeType::TTLDrop;
}

}
