#pragma once

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cctype>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>

namespace stdin_fair {

    //===========================
    // 1) Normalization helpers
    //===========================
    inline void rtrim(std::string& s) {
        while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t'))
            s.pop_back();
    }
    inline void normalize(std::string& s) {
        rtrim(s);
        for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    inline bool match_normalized(std::string a, std::string b) {
        normalize(a); normalize(b);
        return a == b;
    }

    //===========================
    // 2) Gate: fairness (tickets)
    //===========================
    struct Gate {
        std::mutex m;
        std::condition_variable cv;
        unsigned long long next_ticket = 0;  // next ticket to hand out
        unsigned long long serving = 0;  // currently served ticket
    };
    inline Gate g; // single instance across all TUs (C++17 inline variable)

    struct Turn {
        bool ok = false;
        unsigned long long ticket = 0;
        std::unique_lock<std::mutex> lock; // holds g.m when ok==true
    };

    // Acquire a turn; returns with the lock held on success.
    // 'deadline' applies only to waiting for our turn (not stdin I/O).
    inline Turn acquire_turn(std::atomic<bool>* cancel,
        std::chrono::steady_clock::time_point deadline)
    {
        using namespace std::chrono;
        std::unique_lock<std::mutex> lk(g.m);
        const unsigned long long my_ticket = g.next_ticket++;

        // Wait until it's our turn or we time out/cancel
        if (!g.cv.wait_until(lk, deadline, [&] {
            return g.serving == my_ticket || (cancel && cancel->load());
            })) {
            return { false, my_ticket, std::move(lk) }; // timed out
        }
        if (cancel && cancel->load()) return { false, my_ticket, std::move(lk) };
        if (g.serving != my_ticket)   return { false, my_ticket, std::move(lk) };

        return { true, my_ticket, std::move(lk) }; // success; lock is held
    }

    inline void release_turn_and_notify(std::unique_lock<std::mutex>& lk) {
        ++g.serving;
        lk.unlock();
        g.cv.notify_one();
    }

    //=====================================
    // 3) Machine store (inventory of cmds)
    //=====================================
    struct MachineStore {
        // counts per normalized command
        std::unordered_map<std::string, int> counts;

        void clear() { counts.clear(); }

        void add(std::string cmd, int n = 1) {
            normalize(cmd);
            counts[cmd] += n;
        }

        bool consume_if_available(const std::string& expected_norm) {
            auto it = counts.find(expected_norm);
            if (it == counts.end() || it->second == 0) return false;
            if (--(it->second) == 0) counts.erase(it);
            return true;
        }

        std::string snapshot() const {
            std::string s = "{ ";
            for (const auto& kv : counts) {
                s += kv.first;
                s += ":";
                s += std::to_string(kv.second);
                s += " ";
            }
            s += "}";
            return s;
        }
    };
    inline MachineStore machine;

    // Convenience preload/reset
    inline void reset(bool clear_machine = true) {
        std::lock_guard<std::mutex> lk(g.m);
        g.next_ticket = g.serving = 0;
        if (clear_machine) machine.clear();
    }
    inline void preload(std::initializer_list<std::string> cmds) {
        std::lock_guard<std::mutex> lk(g.m);
        for (auto s : cmds) machine.add(std::move(s), 1);
    }
    inline void push_token(std::string cmd, int n = 1) {
        std::lock_guard<std::mutex> lk(g.m);
        machine.add(std::move(cmd), n);
    }

    //=============================
    // 4) Human I/O (prompt+getline)
    //=============================
    inline bool prompt_and_read_line(const std::string& prompt, std::string& out_line) {
        std::cerr << "\n" << prompt << "\n" << std::flush;
        if (!std::getline(std::cin, out_line)) return false;
        return true;
    }

    //==================================================
    // 5A) Orchestrator: MACHINE (no stdin, tokens only)
    //     - consumes a matching token if available
    //     - otherwise yields up to 'tries' times if others are queued
    //     - then asserts (or returns false) when still unavailable
    //==================================================
    inline bool WaitForCommandMachine(const std::string& expected_raw,
        int tries = 1,
        std::atomic<bool>* cancel = nullptr,
        std::chrono::milliseconds timeout = std::chrono::milliseconds::zero(),
        bool assert_on_fail = true)
    {
        using namespace std::chrono;
        const auto deadline = (timeout == milliseconds::zero())
            ? (steady_clock::time_point::max)()
            : (steady_clock::now() + timeout);

        std::string expected_norm = expected_raw;
        normalize(expected_norm);

        // Acquire first turn
        Turn turn = acquire_turn(cancel, deadline);
        if (!turn.ok) return false; // timed out or canceled before our first turn

        int yields_left = (tries < 0 ? 0 : tries);

        for (;;) {
            // We hold the lock and it's our turn.
            // Try to consume our token from the machine store (protected by the same lock).
            if (machine.consume_if_available(expected_norm)) {
                release_turn_and_notify(turn.lock);
                return true;
            }

            // No token for us. Yield (requeue once per try) if others are waiting.
            const bool others_waiting = (g.next_ticket > g.serving + 1);
            if (others_waiting && yields_left > 0) {
                --yields_left;

                // Requeue ourselves: get a new ticket at the back,
                // advance 'serving' to let next thread run, then wait for our new turn.
                const unsigned long long new_ticket = g.next_ticket++;
                ++g.serving;
                g.cv.notify_one();

                // Wait for new ticket with the same deadline/cancel semantics.
                if (!g.cv.wait_until(turn.lock, deadline, [&] {
                    return g.serving == new_ticket || (cancel && cancel->load());
                    })) {
                    // timed out while requeued
                    turn.ok = false;
                    return false;
                }
                if (cancel && cancel->load()) { turn.ok = false; return false; }
                // Now it's our turn again; loop and retry token consumption.
                continue;
            }

            // Out of yields (or nobody else waiting): fail this step.
            const std::string snap = machine.snapshot();
            release_turn_and_notify(turn.lock);

            if (assert_on_fail) {
                std::cerr << "[machine] Missing token for \"" << expected_norm
                    << "\"; inventory " << snap
                    << "; tries exhausted=" << tries << "\n";
                assert(false && "WaitForCommandMachine: expected token not available");
            }
            return false;
        }
    }

    //======================================================
    // 5B) Orchestrator: HUMAN (stdin; drop mismatched input)
    //     - waits for turn, prompts, reads a line
    //     - returns true if it matches; false otherwise
    //     - mismatches are dropped (never recycled)
    //======================================================
    inline bool WaitForCommandHumanOnce(const std::string& prompt,
        const std::string& expected_raw,
        std::atomic<bool>* cancel = nullptr,
        std::chrono::milliseconds timeout = std::chrono::milliseconds::zero())
    {
        using namespace std::chrono;
        const auto deadline = (timeout == milliseconds::zero())
            ? (steady_clock::time_point::max)()
            : (steady_clock::now() + timeout);

        std::string expected_norm = expected_raw;
        normalize(expected_norm);

        // Acquire turn
        Turn turn = acquire_turn(cancel, deadline);
        if (!turn.ok) return false; // timed out / canceled while waiting

        // Do I/O without holding the mutex (nobody can advance, serving==our ticket).
        turn.lock.unlock();

        std::string line;
        const bool io_ok = prompt_and_read_line(prompt, line);
        if (io_ok) normalize(line);

        // Relock, release our turn, and notify the next waiter.
        turn.lock.lock();
        release_turn_and_notify(turn.lock);

        if (!io_ok) return false;
        return (line == expected_norm);
    }

    // Convenience: loop until a human line matches (mismatches dropped).
    inline bool WaitForCommandHuman(const std::string& prompt,
        const std::string& expected_raw,
        std::atomic<bool>* cancel = nullptr,
        std::chrono::milliseconds timeout = std::chrono::milliseconds::zero())
    {
        for (;;) {
            if (WaitForCommandHumanOnce(prompt, expected_raw, cancel, timeout))
                return true;
            // else: drop and keep asking
        }
    }

    //============================
    // 6) Compatibility wrapper
    //============================
    enum class Mode { HumanOnly, MachineOnly };

    inline bool WaitForCommandScript(const std::string& prompt,
        const std::string& expected,
        std::atomic<bool>* cancel = nullptr,
        std::chrono::milliseconds timeout = std::chrono::milliseconds::zero(),
        int machine_tries = 1,
        Mode mode = Mode::MachineOnly)
    {
        if (mode == Mode::HumanOnly) {
            return WaitForCommandMachine(expected, machine_tries, cancel, timeout, /*assert_on_fail=*/true);
        }
        else {
            return WaitForCommandHuman(prompt, expected, cancel, timeout);
        }
    }

    //============================
    // 7) Tiny debug helper
    //============================
    inline void dump_state(std::ostream& os = std::cerr) {
        std::lock_guard<std::mutex> lk(g.m);
        os << "serving=" << g.serving << " next=" << g.next_ticket
            << " inventory=" << machine.snapshot() << "\n";
    }

} // namespace stdin_fair
