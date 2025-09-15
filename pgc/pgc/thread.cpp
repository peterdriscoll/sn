#include "thread.h"

#include <atomic>
#include <limits>

#include "pgc_pch.h"

namespace PGC
{
	namespace Thread
	{
		static std::atomic<std::size_t> g_next_id{ 0 };
		// Each OS thread gets a slot the first time it calls get_thread_num()
		thread_local std::size_t t_thread_num = std::numeric_limits<std::size_t>::max();

		std::size_t get_thread_num() noexcept
		{
			auto  constexpr uninit = std::numeric_limits<std::size_t>::max();
			if (t_thread_num == uninit) {
				// Relaxed: we only need uniqueness, not ordering guarantees.
				t_thread_num = g_next_id.fetch_add(1, std::memory_order_relaxed);
			}
			return t_thread_num;
		}

		void reset_thread_numbers_for_tests() noexcept
		{
			g_next_id.store(0, std::memory_order_relaxed);
			// NOTE: existing threads keep their t_thread_num until the TLS is torn down.
			// Use only in single-threaded test setups before starting worker threads.
		}
	}
} // namespace PGC::Threads
