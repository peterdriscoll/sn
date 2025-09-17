#pragma once

#include "exp_ctrl_pgc.h"
#include "thread.h"

#include <unordered_set>
#include <functional>

namespace PGC
{
    class PGC_EXPORT ThreadRegistry
    {
    public:
        // Register/unregister the *current* TLS PGC::Thread
      PGC_User *RegisterCurrent(PGC_User &user, const char *name = nullptr);
      void UnregisterCurrent(PGC_User *p_LastUser = nullptr);

        // Iterate safely over a snapshot
        void ForEach(const std::function<void(const Thread*)>& fn) const;

        std::size_t size() const;

    private:
        mutable std::mutex m_mutex;
        std::unordered_set<const Thread*> m_ThreadSet;
    };

    // RAII helper: registers on scope entry, unregisters on exit
    class PGC_EXPORT ThreadMembership
    {
      public:
        ThreadRegistry& reg;
        explicit ThreadMembership(PGC_User& user, const char* name = nullptr);
        ~ThreadMembership();

      private:
        PGC_User *m_LastUser = nullptr;
    };

} // namespace PGC
