#include <Interpreters/Cache/FileSegmentKeyType.h>

namespace DB
{

String getKeyTypePrefix(FileSegmentKeyType type)
{
    if (type == FileSegmentKeyType::General)
        return "";
    return toString(type);
}

String toString(FileSegmentKeyType type)
{
    switch (type)
    {
        case FileSegmentKeyType::General:
            return "General";
        case FileSegmentKeyType::System:
            return "System";
        case FileSegmentKeyType::Data:
            return "Data";
    }
}

}
