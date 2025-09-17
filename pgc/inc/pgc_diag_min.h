#pragma once

#include <atomic>
#include <cstdint>
#include <vector>
#include <mutex>
#include <chrono>

#ifndef PGC_DEBUG
// flip this on in Debug | Preprocessor Definitions
// #define PGC_DEBUG
#endif

namespace PGC
{

	// --- time
	inline uint64_t nsec_now()
	{
		using namespace std::chrono;
		return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
	}

	// --- thread glue (adapt to your Thread API)
	struct Thread
	{
		static Thread& GetCurrentThread();     // you already have this
		const char* GetName() const;        // or std::string; adapt printing
		unsigned long  tid() const;            // optional; use address if not present
	};

	// ---------- RELEASE: fast truth (bit-stack) ----------
	struct GuardStack
	{
		uint64_t mask = 0;   // bit i = mode (1=Mutex, 0=UnGuard)
		uint8_t  depth = 0;
		inline void push(bool lock) { if (lock) mask |= (1ull << depth); else mask &= ~(1ull << depth); ++depth; }
		inline void pop() { --depth; mask &= ~(1ull << depth); }
		inline bool top_locked() const { return depth == 0 ? true : ((mask >> (depth - 1)) & 1ull); }

#ifdef PGC_DEBUG
		struct Frame
		{
			uint64_t id; bool locked;
			const char* label; const char* file; int line;
			const void* key;  uint64_t t_enter_ns;
		};
		std::mutex mu;
		std::vector<Frame> dbg;
		inline void dbg_push(Frame f) { std::lock_guard<std::mutex> lk(mu); dbg.push_back(f); }
		inline void dbg_pop() { std::lock_guard<std::mutex> lk(mu); if (!dbg.empty()) dbg.pop_back(); }
		inline std::vector<Frame> snapshot() const { std::lock_guard<std::mutex> lk(mu); return dbg; }
#endif
	};
	inline thread_local GuardStack g_guards;

	// ---------- RAII guard ----------
	inline std::atomic<uint64_t> g_guard_id{ 1 };

	struct ScopedGuard
	{
		bool locked;
#ifdef PGC_DEBUG
		GuardStack::Frame f;
		ScopedGuard(bool L, const char* lbl, const char* file, int line, const void* key)
			: locked(L), f{ g_guard_id.fetch_add(1), L, lbl, file, line, key, nsec_now() }
		{
			g_guards.push(locked); g_guards.dbg_push(f);
		}
		~ScopedGuard() { g_guards.dbg_pop(); g_guards.pop(); }
#else
		explicit ScopedGuard(bool L) : locked(L) { g_guards.push(locked); }
		~ScopedGuard() { g_guards.pop(); }
#endif
	};

	struct MutexGuard : ScopedGuard
	{
#ifdef PGC_DEBUG
		MutexGuard(const char* lbl, const char* f, int ln, const void* key) : ScopedGuard(true, lbl, f, ln, key) {}
#else
		MutexGuard() : ScopedGuard(true) {}
#endif
	};
	struct UnGuard : ScopedGuard
	{
#ifdef PGC_DEBUG
		UnGuard(const char* lbl, const char* f, int ln, const void* key) : ScopedGuard(false, lbl, f, ln, key) {}
#else
		UnGuard() : ScopedGuard(false) {}
#endif
	};

#ifdef PGC_DEBUG
#define ENGUARD(lbl,key) ::PGC::MutexGuard _g_##__LINE__(lbl,__FILE__,__LINE__,key)
#define UNGUARD(lbl,key) ::PGC::UnGuard    _g_##__LINE__(lbl,__FILE__,__LINE__,key)
#else
#define ENGUARD(lbl,key) ::PGC::MutexGuard _g_##__LINE__()
#define UNGUARD(lbl,key) ::PGC::UnGuard    _g_##__LINE__()
#endif

	// ---------- lock owner meta + reporting ----------
	struct DebugLockMeta
	{
		const char* name = "PGC_Lock";
		PGC::Thread* owner = nullptr;
		const char* owner_file = nullptr; int owner_line = 0;

		inline void on_acquire(const char* f, int l) { owner = &PGC::Thread::GetCurrentThread(); owner_file = f; owner_line = l; }
		inline void on_release() { owner = nullptr; owner_file = nullptr; owner_line = 0; }
	};

	inline void dump_stack(const char* tag, Thread* t)
	{
#ifdef PGC_DEBUG
		auto snap = g_guards.snapshot(); // local TLS if t==current; adapt if you store per-thread
		// If you later store per-thread stacks on the Thread object, swap to: t->guards().snapshot()
		fprintf(stderr, "  %s stack (%zu):\n", tag, snap.size());
		for (size_t i = 0; i < snap.size(); ++i) {
			const auto& f = snap[i];
			fprintf(stderr, "    #%02zu %-6s key=%p at %s:%d \"%s\"\n",
				i + 1, f.locked ? "Mutex" : "UnGd", f.key, f.file, f.line, f.label ? f.label : "");
		}
#else
		(void)tag; (void)t;
#endif
	}

	inline void report_lock_wait(const DebugLockMeta& L, Thread* waiter, uint64_t waited_ms, const char* where_file, int where_line, const char* label)
	{
		fprintf(stderr, "[LOCKOUT] %llums waiter=%s at %s:%d label=\"%s\" on %s\n",
			(unsigned long long)waited_ms, waiter->GetName(), where_file, where_line, (label ? label : ""), L.name);
		if (L.owner) {
			fprintf(stderr, "          owner=%s at %s:%d\n", L.owner->GetName(), L.owner_file, L.owner_line);
			dump_stack("owner", L.owner);
		}
		dump_stack("waiter", waiter);
	}

} // namespace PGC
