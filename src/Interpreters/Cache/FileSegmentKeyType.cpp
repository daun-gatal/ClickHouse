#include <Interpreters/Cache/FileSegmentKeyType.h>

namespace DB
{

namespace
{

std::string_view fileSegmentKeyTypeToStringView(FileSegmentKeyType type)
{
    switch (type)
    {
        case FileSegmentKeyType::General: return "General";
        case FileSegmentKeyType::System: return "System";
        case FileSegmentKeyType::Data: return "Data";
    }
    return "Unknown";
}

}

String getKeyTypePrefix(FileSegmentKeyType type)
{
    if (type == FileSegmentKeyType::General)
        return "";
    return String(fileSegmentKeyTypeToStringView(type));
}

String enumToString(FileSegmentKeyType type)
{
    return String(fileSegmentKeyTypeToStringView(type));
}

}
