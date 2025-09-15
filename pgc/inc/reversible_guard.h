#pragma once

#include <mutex>
#include <vector>
#include <cstddef>
#include "thread.h"

namespace PGC
{
	struct ReversibleMutex;
	// -----------------------------------------
	// Per-mutex, per-thread reversible mode data
	// -----------------------------------------
	struct ReversibleState
	{
		// mode_stack[tid] is a stack (vector<bool>) of modes for that thread.
		// false = "unlocked" desire; true = "locked" desire
		std::vector<std::vector<bool>> mode_stack;
		std::mutex grow_mtx; // protects growth of mode_stack

		void ensure(std::size_t tid)
		{
			if (tid < mode_stack.size()) return;
			std::lock_guard<std::mutex> lk(grow_mtx);
			if (tid >= mode_stack.size()) {
				mode_stack.resize(tid + 1);
			}
		}
	};

	// ------------------------------------------------------
	// ReversibleGuard: flips OS mutex only when mode changes
	// ------------------------------------------------------
	// LockIt=true  => EnGuard (desire "locked")
	// LockIt=false => UnGuard (desire "unlocked")
	template<bool LockIt = true>
	class ReversibleGuard
	{
	public:
		explicit ReversibleGuard(ReversibleMutex& mtx, ReversibleState& state)
			: m_mtx(mtx)
			, m_state(state)
			, m_tid(Thread::get_thread_num())
		{
			m_state.ensure(m_tid);
			auto& stack = m_state.mode_stack[m_tid];

			const bool prior = stack.empty() ? false /*unlocked*/ : stack.back();
			constexpr bool want = LockIt;

			if (want != prior) {
				if (want) m_mtx.lock();
				else      m_mtx.unlock(); // This unlock is only called by the thread that previously locked.
			}
			stack.push_back(want);
		}

		~ReversibleGuard()
		{
			// Pop current and restore prior mode if different.
			auto& stack = m_state.mode_stack[m_tid];

			const bool current = (!stack.empty()) ? stack.back() : false;
			if (!stack.empty()) stack.pop_back();

			const bool prior = stack.empty() ? false : stack.back();

			if (current != prior) {
				if (prior) m_mtx.lock();
				else       m_mtx.unlock();
			}
		}

		ReversibleGuard(const ReversibleGuard&) = delete;
		ReversibleGuard& operator=(const ReversibleGuard&) = delete;
		ReversibleGuard(ReversibleGuard&&) = delete;
		ReversibleGuard& operator=(ReversibleGuard&&) = delete;

	private:
		ReversibleMutex& m_mtx;
		ReversibleState& m_state;
		size_t           m_tid;
	};

	// Friendly aliases for std::mutex
	using EnGuard = ReversibleGuard<true>;
	using UnGuard = ReversibleGuard<false>;
	// ----------------------------------------------------
	// A tiny helper wrapper if you want a self-contained
	// reversible std::mutex-like object.
	// ----------------------------------------------------
	struct ReversibleMutex
	{
		std::mutex     mtx;
		ReversibleState state;

		void lock() { mtx.lock(); };
		void unlock() { mtx.unlock(); };

		// RAII tokens (thread id is captured internally)
		EnGuard  lock_token() { return EnGuard(*this, state); };
		UnGuard  unlock_token() { return UnGuard(*this, state); };
	};

} // namespace PGC
