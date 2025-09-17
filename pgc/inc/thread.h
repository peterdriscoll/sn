#pragma once

#include "exp_ctrl_pgc.h"

namespace PGC
{
    class PGC_EXPORT Thread
    {
    public:
        static Thread& GetCurrentThread() noexcept;
        static Thread* GetCurrentThreadPtr() noexcept;

        // Optional: reset numbering (useful in tests); not thread-safe while threads alive.
        static void reset_thread_numbers_for_tests() noexcept;

        Thread() noexcept;
		~Thread() noexcept;

        PGC_User& GetUser() const noexcept;
        void SetUser(PGC_User &p_User) noexcept;
        std::size_t GetId() const noexcept;
        void SetId(std::size_t p_Id) noexcept;
        std::string GetName() const noexcept;
        void SetName(const std::string& p_name) noexcept;
    private:
        // Returns the small, ephemeral thread id (0,1,2,...) for the calling thread.
        // One per process when linked from a single binary or shared DLL.
        static std::size_t get_thread_num() noexcept;

        static std::atomic<std::size_t> g_next_id;

        PGC_User* m_User;
        std::string m_name;
        std::size_t m_Id;
    };
} // namespace PGC::Threads
