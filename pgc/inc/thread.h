#pragma once

#include "exp_ctrl_pgc.h"

namespace PGC
{
    namespace Thread
    {

        // Returns the small, ephemeral thread id (0,1,2,...) for the calling thread.
        // One per process when linked from a single binary or shared DLL.
        PGC_EXPORT std::size_t get_thread_num() noexcept;

        // Optional: reset numbering (useful in tests); not thread-safe while threads alive.
        PGC_EXPORT void reset_thread_numbers_for_tests() noexcept;
    }
} // namespace PGC::Threads
