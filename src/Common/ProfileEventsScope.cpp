#include <Common/ProfileEventsScope.h>
#include <Common/CurrentThread.h>
#include <Common/VariableContext.h>

namespace DB
{

ProfileEventScopeExtension::ProfileEventScopeExtension(ProfileEvents::CountersPtr to)
    : attached_scope(std::move(to))
{
    CurrentThread::get().attachProfileCountersScope(attached_scope);
}

ProfileEventScopeExtension::~ProfileEventScopeExtension()
{
    auto detached_scope = CurrentThread::get().detachProfileCountersScope();
    chassert(detached_scope == attached_scope);
}

ProfileEventsScope::ProfileEventsScope()
    : performance_counters_holder(VariableContext::Scope)
{
}

std::shared_ptr<ProfileEventsScope> ProfileEventsScope::construct()
{
    struct ProfileEventsScopeCreator : public ProfileEventsScope {};
    return std::make_shared<ProfileEventsScopeCreator>();
}

ProfileEventScopeExtension ProfileEventsScope::startCollecting()
{
    return ProfileEventScopeExtension(getCounters());
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
