#pragma once

#include <Common/ProfileEvents.h>

#include <memory>

namespace DB
{

class ProfileEventScopeExtension
{
public:
    explicit ProfileEventScopeExtension(ProfileEvents::CountersPtr to);
    ~ProfileEventScopeExtension();

private:
    ProfileEvents::CountersPtr attached_scope;
};

/// Use specific performance counters for current thread in the current scope.
class ProfileEventsScope : public std::enable_shared_from_this<ProfileEventsScope>
{
    explicit ProfileEventsScope();

public:
    static std::shared_ptr<ProfileEventsScope> construct();

    ProfileEventScopeExtension startCollecting();
    ProfileEvents::CountersPtr getCounters();
    std::shared_ptr<ProfileEvents::Counters::Snapshot> getSnapshot();

private:
    ProfileEvents::Counters performance_counters_holder;
};

using ProfileEventsScopePtr = std::shared_ptr<ProfileEventsScope>;

}

