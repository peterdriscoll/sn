#pragma once

#include "exp_ctrl_pgc.h"

namespace PGC
{
    namespace Thread
    {

        // Returns the small, ephemeral thread id (0,1,2,...) for the calling thread.
        // One per process when linked from a single binary or shared DLL.
        PGC_EXPORT static std::size_t get_thread_num() noexcept;

        // Optional: reset numbering (useful in tests); not thread-safe while threads alive.
        PGC_EXPORT static void reset_thread_numbers_for_tests() noexcept;

        Thread() noexcept;
		~Thread() noexcept;

        std::string GetName() const noexcept;
		void SetName(const std::string &p_name) noexcept;
    private:
        static std::atomic<std::size_t> g_next_id;

        std::string m_name;
        long m_Id;
    };
} // namespace PGC::Threads
