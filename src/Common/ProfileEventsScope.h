#pragma once

#include <Common/ProfileEvents.h>

#include <memory>

namespace DB
{

class ProfileEventsScopeSwitch
{
public:
    explicit ProfileEventsScopeSwitch(ProfileEvents::CountersPtr to);
    ~ProfileEventsScopeSwitch();

private:
    ProfileEvents::CountersPtr previous_counters_scope;
};

/// Use specific performance counters for current thread in the current scope.
class ProfileEventsScope : public std::enable_shared_from_this<ProfileEventsScope>
{
public:
    ProfileEventsScopeSwitch startCollecting();
    ProfileEvents::CountersPtr getCounters();
    std::shared_ptr<ProfileEvents::Counters::Snapshot> getSnapshot();

private:
    ProfileEvents::Counters performance_counters_holder;
};

using ProfileEventsScopePtr = std::shared_ptr<ProfileEventsScope>;

}

