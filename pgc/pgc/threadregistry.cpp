#include "threadregistry.h"
#include "thread.h"
#include <mutex>

#include "pgc_pch.h"

namespace PGC
{
    PGC_User* ThreadRegistry::RegisterCurrent(PGC_User& user, const char* name)
    {
        Thread& t = Thread::GetCurrentThread();
        PGC_User *lastUser = &(t.GetUser());
        t.SetUser(user);
        if (name) t.SetName(name);
        std::lock_guard<std::mutex> lk(m_mutex);
        m_ThreadSet.insert(&t);
        return lastUser;
    }

    void ThreadRegistry::UnregisterCurrent(PGC_User *p_LastUser)
    {
        Thread& t = Thread::GetCurrentThread();
        t.SetUser(*p_LastUser);
        std::lock_guard<std::mutex> lk(m_mutex);
        m_ThreadSet.erase(&t);
    }

    // Iterate safely over a snapshot
    void ThreadRegistry::ForEach(const std::function<void(const Thread*)>& fn) const
    {
        std::vector<const Thread*> snap;
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            snap.reserve(m_ThreadSet.size());
            for (auto* p : m_ThreadSet) snap.push_back(p);
        }
        for (auto* p : snap) fn(p);
    }

    std::size_t ThreadRegistry::size() const
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_ThreadSet.size();
    }

    ThreadMembership::ThreadMembership(PGC_User& user, const char* name)
        : reg(user.GetThreadRegistry())
    {
        m_LastUser = reg.RegisterCurrent(user, name);
    }

    ThreadMembership::~ThreadMembership()
    { 
        reg.UnregisterCurrent(m_LastUser);
    }

} // namespace PGC