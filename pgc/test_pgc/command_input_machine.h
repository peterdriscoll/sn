/*------------------------------------------------------------------------------
  command_input_machine.h  —  Strict FIFO scripted command queue (deterministic)

  What it is:
    - A minimal, deterministic harness for machine-driven tests.
    - Consumes tokens ONLY when expected == queue.front().
    - No stdin path; no accidental interleaving.

  Guarantees:
    - Strict FIFO consumption.
    - Cooperative waiting with a retry/fail budget per token.
    - Clean logs (no automatic spew into your data stream).

  Tradeoffs:
    - No interactive stdin. Pure machine control by design.

  When to use:
    - CI, reproducible tests, protocol verification, and any run where order
      must be exact.

  Minimal API:
    void reset() noexcept;                                        // clear state
    void set_default_fails(int fails) noexcept;                   // e.g., 1000
    void set_yield_sleep(std::chrono::milliseconds) noexcept;     // e.g., 0–1ms
    void preload(std::initializer_list<const char*> tokens);      // seed script
    void push(std::string token);                                 // inject at tail
    void WaitForCommandScript(const char* who, const char* expected);
      // Blocks until queue.front() == expected; otherwise yields and retries.
      // Decrements per-item fail budget; throws/logs when exhausted.

    void dump_pending_to_stderr() noexcept;  // optional debug; NEVER use main os

  Tips:
    - Keep JSON/structured output separate from diagnostics (stderr).
    - If you need human input for a one-off run, switch temporarily to the
      hybrid variant, but don’t use it in CI.

------------------------------------------------------------------------------*/


#pragma once

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifndef STDIN_FAIR_DEBUG_BREAK
#if defined(_MSC_VER)
#define STDIN_FAIR_DEBUG_BREAK() __debugbreak()
#else
#include <cstdlib>
#define STDIN_FAIR_DEBUG_BREAK() std::abort()
#endif
#endif

namespace stdin_fair {

    //===============================
    // Ordered script (machine mode)
    //===============================
    struct ScriptEntry {
        std::string text;
        int         fails_remaining;   // decremented on each mismatch observation
        std::uint64_t seqno;           // unique id to suppress repeat logs
    };

    struct ScriptQueue {
        std::mutex m;
        std::deque<ScriptEntry> q;
        std::uint64_t next_seqno = 1;
        std::uint64_t last_reported_seqno = 0; // to avoid repeated info for same front
        int           default_fails = 1000;    // generous by default
        std::chrono::milliseconds yield_sleep{ 0 }; // optional tiny sleep after yield

        void clear() {
            std::lock_guard<std::mutex> lk(m);
            q.clear();
            next_seqno = 1;
            last_reported_seqno = 0;
        }

        // Append one command with explicit fails
        void push_back(std::string cmd, int fails) {
            std::lock_guard<std::mutex> lk(m);
            q.push_back(ScriptEntry{ std::move(cmd), fails, next_seqno++ });
        }

        // Append several with same fails
        void preload(std::initializer_list<std::string> cmds, int fails_each = 1000) {
            std::lock_guard<std::mutex> lk(m);
            for (auto& s : cmds) {
                q.push_back(ScriptEntry{ std::string(s), fails_each, next_seqno++ });
            }
        }

        // Append several with per-item fails
        void preload_with_fails(std::initializer_list<std::pair<std::string, int>> cmds) {
            std::lock_guard<std::mutex> lk(m);
            for (auto& p : cmds) {
                q.push_back(ScriptEntry{ p.first, p.second, next_seqno++ });
            }
        }

        // Debug helper: dump a compact snapshot
        void dump_queue(std::ostream& os = std::cerr, std::size_t preview = 6) {
            std::lock_guard<std::mutex> lk(m);
            os << "[queue size=" << q.size() << "] ";
            std::size_t n = (std::min)(preview, q.size());
            for (std::size_t i = 0; i < n; ++i) {
                os << (i == 0 ? "[front " : "[")
                    << i << "]\"" << q[i].text << "\""
                    << "(fails=" << q[i].fails_remaining << ",#" << q[i].seqno << ") ";
            }
            if (q.size() > n) os << "...";
            os << "\n";
        }
    };

    // One shared queue across all TUs (C++17)
    inline ScriptQueue g_script;

    // Reset queue & knobs
    inline void reset() { g_script.clear(); }
    inline void set_default_fails(int n) {
        std::lock_guard<std::mutex> lk(g_script.m);
        g_script.default_fails = n;
    }
    inline void set_yield_sleep(std::chrono::milliseconds d) {
        std::lock_guard<std::mutex> lk(g_script.m);
        g_script.yield_sleep = d;
    }
    inline void preload(std::initializer_list<std::string> cmds, int fails_each = 1000) {
        g_script.preload(cmds, fails_each);
    }
    inline void preload_with_fails(std::initializer_list<std::pair<std::string, int>> cmds) {
        g_script.preload_with_fails(cmds);
    }
    inline void push_back(std::string cmd, int fails = -1) {
        if (fails < 0) {
            std::lock_guard<std::mutex> lk(g_script.m);
            g_script.q.push_back(ScriptEntry{ std::move(cmd), g_script.default_fails, g_script.next_seqno++ });
        }
        else {
            g_script.push_back(std::move(cmd), fails);
        }
    }
    inline void dump_queue(std::ostream& os = std::cerr, std::size_t preview = 6) {
        g_script.dump_queue(os, preview);
    }

    // Find the first index of expected in the queue; returns -1 if absent.
    // Requires caller to hold g_script.m.
    inline int find_expected_index_locked(const std::string& expected) {
        for (std::size_t i = 0; i < g_script.q.size(); ++i) {
            if (g_script.q[i].text == expected) return static_cast<int>(i);
        }
        return -1;
    }

    // Core machine API: strictly ordered consumption, no prompting.
    // Returns true when the front equals expected and is consumed.
    // On persistent mismatch (fails reaches 0) triggers debug break (or assert).
    inline bool WaitForCommandScript(const std::string prompt, const std::string& expected) {
        for (;;) {
            // ---- critical section: inspect/update front entry ----
            std::unique_lock<std::mutex> lk(g_script.m);

            if (g_script.q.empty()) {
                std::cerr << "[machine] script exhausted; expected=\"" << expected << "\"\n";
                STDIN_FAIR_DEBUG_BREAK();
                return false; // unreachable after abort/break, but keeps compiler happy
            }

            ScriptEntry& front = g_script.q.front();

            if (front.text == expected) {
                // Consume and exit
                g_script.q.pop_front();
                // Move last_reported_seqno forward so we report on the next new front if mismatch later.
                g_script.last_reported_seqno = 0;
                lk.unlock();
                return true;
            }

            // Mismatch: log once per front seqno
            if (front.seqno != g_script.last_reported_seqno) {
                const int expected_pos = find_expected_index_locked(expected);
                std::cerr << "[machine] mismatch: expected=\"" << expected << "\""
                    << " front=\"" << front.text << "\""
                    << " fails_remaining=" << front.fails_remaining
                    << " front_seq=" << front.seqno
                    << " expected_index=" << expected_pos << "\n";
                // Optional preview of next few items
                std::size_t preview = std::min<std::size_t>(5, g_script.q.size());
                std::cerr << "         queue_next: ";
                for (std::size_t i = 0; i < preview; ++i) {
                    std::cerr << (i == 0 ? "[0]" : "[")
                        << i << "]\"" << g_script.q[i].text << "\"";
                    if (i + 1 < preview) std::cerr << ", ";
                }
                if (g_script.q.size() > preview) std::cerr << ", ...";
                std::cerr << "\n";

                g_script.last_reported_seqno = front.seqno;
            }

            // Decrement fails_remaining and decide
            if (--front.fails_remaining <= 0) {
                const int expected_pos = find_expected_index_locked(expected);
                std::cerr << "[machine] FAIL: expected=\"" << expected << "\""
                    << " front=\"" << front.text << "\""
                    << " expected_index=" << expected_pos
                    << " (giving up after too many mismatches)\n";
                lk.unlock();
                STDIN_FAIR_DEBUG_BREAK();
                return false;
            }

            // Done with shared state for this pass
            lk.unlock();

            // ---- cooperative yield outside lock ----
            std::this_thread::yield();
            auto sleep = g_script.yield_sleep;
            if (sleep.count() > 0) std::this_thread::sleep_for(sleep);
            // loop
        }
    }

} // namespace stdin_fair
