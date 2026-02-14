#include <Common/ProfileEventsScope.h>
#include <Common/CurrentThread.h>

namespace DB
{

ProfileEventsScopeSwitch::ProfileEventsScopeSwitch(ProfileEvents::CountersPtr to)
{
    previous_counters_scope = CurrentThread::get().attachProfileCountersScope(to);
}

ProfileEventsScopeSwitch::~ProfileEventsScopeSwitch()
{
    CurrentThread::get().attachProfileCountersScope(previous_counters_scope);
}

ProfileEventsScopeSwitch ProfileEventsScope::startCollecting()
{
    return ProfileEventsScopeSwitch(getCounters());
}

ProfileEvents::CountersPtr ProfileEventsScope::getCounters()
{
    return ProfileEvents::CountersPtr(shared_from_this(), &performance_counters_holder);
}

std::shared_ptr<ProfileEvents::Counters::Snapshot> ProfileEventsScope::getSnapshot()
{
    return std::make_shared<ProfileEvents::Counters::Snapshot>(performance_counters_holder.getPartiallyAtomicSnapshot());
}

}
