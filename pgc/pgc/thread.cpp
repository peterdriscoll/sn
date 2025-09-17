#include "thread.h"

#include <atomic>
#include <limits>

#include "pgc_pch.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <pthread.h>
    #include <sys/prctl.h>
#elif defined(__APPLE__)
    #include <pthread.h>
#endif

namespace PGC
{
    inline void set_current_thread_name(std::string_view name)
    {
#if defined(_WIN32)
        // Convert UTF-8 -> UTF-16 for SetThreadDescription
        int wlen = ::MultiByteToWideChar(CP_UTF8, 0, name.data(),
                                         (int)name.size(), nullptr, 0);
        if (wlen <= 0)
            return;
        std::wstring wname(static_cast<size_t>(wlen), L'\0');
        ::MultiByteToWideChar(CP_UTF8, 0, name.data(), (int)name.size(),
                              wname.data(), wlen);
        ::SetThreadDescription(::GetCurrentThread(),
                               wname.c_str()); // ignore HR for brevity
#elif defined(__APPLE__)
        // macOS: 64 bytes including NUL, only for *current* thread
        constexpr size_t kMax = 63;
        char buf[kMax + 1];
        size_t n = std::min(name.size(), kMax);
        std::memcpy(buf, name.data(), n);
        buf[n] = '\0';
        ::pthread_setname_np(buf);
#elif defined(__linux__)
        // Linux: 16 bytes including NUL
        constexpr size_t kMax = 15;
        char buf[kMax + 1];
        size_t n = std::min(name.size(), kMax);
        std::memcpy(buf, name.data(), n);
        buf[n] = '\0';
        // prctl works on current thread; avoids glibc signature differences
        ::prctl(PR_SET_NAME, buf, 0, 0, 0);
#else
        (void)name; // no-op on unknown platforms
#endif
    }

    /*static*/ std::atomic<std::size_t> Thread::g_next_id{0};
    // Each OS thread gets a slot the first time it calls get_thread_num()
    thread_local std::size_t t_thread_num =
        (std::numeric_limits<std::size_t>::max)();

    thread_local Thread t_thread;

    Thread &Thread::GetCurrentThread() noexcept
    {
        return t_thread;
    }

    Thread *Thread::GetCurrentThreadPtr() noexcept
    {
        return &t_thread;
    }

    /*static*/ std::size_t Thread::get_thread_num() noexcept
    {
        auto constexpr uninit = (std::numeric_limits<std::size_t>::max)();
        if (t_thread_num == uninit)
        {
            // Relaxed: we only need uniqueness, not ordering guarantees.
            t_thread_num = g_next_id.fetch_add(1, std::memory_order_relaxed);
        }
        return t_thread_num;
    }

    /*static*/ void Thread::reset_thread_numbers_for_tests() noexcept
    {
        g_next_id.store(0, std::memory_order_relaxed);
        // NOTE: existing threads keep their t_thread_num until the TLS is torn
        // down. Use only in single-threaded test setups before starting worker
        // threads.
    }

    Thread::Thread() noexcept
        : m_User(new PGC_User()),
          m_name("unnamed")
    {
        m_Id = get_thread_num();
    }

    Thread::~Thread() noexcept
    {
    }

    PGC_User &Thread::GetUser() const noexcept
    {
        return *m_User;
    }

    void Thread::SetUser(PGC_User &p_User) noexcept
    {
        m_User = &p_User;
    }

    std::size_t Thread::GetId() const noexcept
    {
        return m_Id;
    }
    void Thread::SetId(std::size_t p_Id) noexcept
    {
        m_Id = p_Id;
    }

    std::string Thread::GetName() const noexcept
    {
        return m_name;
    }
    void Thread::SetName(const std::string &p_name) noexcept
    {
        m_name = p_name;
        set_current_thread_name(m_name);
    }
} // namespace PGC
