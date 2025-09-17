#pragma once

#include "thread.h"
#include <stdio.h>

#define ACQUIRE(lockmeta, mtx) \
  ([&]{ (mtx).lock(); (lockmeta).on_acquire(__FILE__, __LINE__); return true; }())

#define RELEASE(lockmeta, mtx) \
  do { (lockmeta).on_release(); (mtx).unlock(); } while(0)

namespace PGC
{
    // DebugLockMeta.h (debug-only fields are fine even in release)
    struct DebugLockMeta
    {
        const char* name = "PGC_Lock";
        PGC::Thread* owner = nullptr;
        const char* owner_file = nullptr; int owner_line = 0;

        inline void on_acquire(const char* f, int l)
        {
            owner = &PGC::Thread::GetCurrentThread();
            owner_file = f; owner_line = l;
        }
        inline void on_release()
        {
            owner = nullptr; owner_file = nullptr; owner_line = 0;
        }
    };

    void report_lock_wait(const DebugLockMeta& L)
    {
        auto* owner = L.owner;
        if (!owner) {
            fprintf(stderr, "[LOCK] waiting on '%s' (no owner)\n", L.name);
            return;
        }
        fprintf(stderr, "[LOCK] waiting on '%s' held by %s at %s:%d\n",
            L.name, owner->GetName().c_str(), L.owner_file, L.owner_line);
    }

}
