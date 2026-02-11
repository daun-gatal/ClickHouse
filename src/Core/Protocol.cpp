#include <Core/Protocol.h>

namespace DB::Protocol
{

namespace Server
{

std::string_view toString(UInt64 packet)
{
    switch (packet)
    {
        case Hello:                         return "Hello";
        case Data:                          return "Data";
        case Exception:                     return "Exception";
        case Progress:                      return "Progress";
        case Pong:                          return "Pong";
        case EndOfStream:                   return "EndOfStream";
        case ProfileInfo:                   return "ProfileInfo";
        case Totals:                        return "Totals";
        case Extremes:                      return "Extremes";
        case TablesStatusResponse:          return "TablesStatusResponse";
        case Log:                           return "Log";
        case TableColumns:                  return "TableColumns";
        case PartUUIDs:                     return "PartUUIDs";
        case ReadTaskRequest:               return "ReadTaskRequest";
        case ProfileEvents:                 return "ProfileEvents";
        case MergeTreeAllRangesAnnouncement: return "MergeTreeAllRangesAnnouncement";
        case MergeTreeReadTaskRequest:      return "MergeTreeReadTaskRequest";
        case TimezoneUpdate:                return "TimezoneUpdate";
        case SSHChallenge:                  return "SSHChallenge";
    }
    return "Unknown packet";
}

}

namespace Client
{

std::string_view toString(UInt64 packet)
{
    switch (packet)
    {
        case Hello:                         return "Hello";
        case Query:                         return "Query";
        case Data:                          return "Data";
        case Cancel:                        return "Cancel";
        case Ping:                          return "Ping";
        case TablesStatusRequest:           return "TablesStatusRequest";
        case KeepAlive:                     return "KeepAlive";
        case Scalar:                        return "Scalar";
        case IgnoredPartUUIDs:              return "IgnoredPartUUIDs";
        case ReadTaskResponse:              return "ReadTaskResponse";
        case MergeTreeReadTaskResponse:     return "MergeTreeReadTaskResponse";
        case SSHChallengeRequest:           return "SSHChallengeRequest";
        case SSHChallengeResponse:          return "SSHChallengeResponse";
        case QueryPlan:                     return "QueryPlan";
    }
    return "Unknown packet";
}

}

}
