#pragma once

#include <deque>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cctype>

namespace stdin_fair {

    // -------- string helpers --------
    inline void rtrim(std::string& s) {
        while (!s.empty() && (s.back()=='\r' || s.back()=='\n' || s.back()==' ' || s.back()=='\t')) s.pop_back();
    }
    inline void normalize(std::string& s) {
        rtrim(s);
        for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    inline bool match_normalized(std::string a, std::string b) {
        normalize(a); normalize(b); return a == b;
    }

    // -------- core types --------
    struct Saved {
        std::string text;
        unsigned long long expire_at; // turn (g.serving) when this entry expires
    };

    struct Gate {
        std::mutex m;
        std::condition_variable cv;
        unsigned long long next_ticket = 0;  // next to hand out
        unsigned long long serving     = 0;  // currently served
        std::deque<Saved> pending;           // saved / preloaded commands
    };

    // single instance across all TUs (C++17+)
    inline Gate g;


    inline void dump_pending(std::ostream& os = std::cerr) {
        std::lock_guard<std::mutex> lk(g.m);
        os << "[pending=" << g.pending.size() << "] { ";
        for (auto const& s : g.pending) os << '"' << s.text << "\"@" << s.expire_at << " ";
        os << "}\n";
    }

    // -------- API --------
    inline bool WaitForCommandScript(const std::string& prompt,
                                   const std::string& expected_raw,
                                   std::atomic<bool>* cancel = nullptr,
                                   std::chrono::milliseconds timeout = std::chrono::milliseconds::zero(),
                                   unsigned int pass_limit = 3)
    {
        using namespace std::chrono;
        bool matched = false, ok = true;

	    dump_pending();

        // compute a single deadline to respect timeout across yields
        using TP = steady_clock::time_point;
        const TP deadline = (timeout == milliseconds::zero())
                          ? (TP::max)()
                          : (steady_clock::now() + timeout);

        // one-time yield budget per call (prevents ping-pong)
        int yields_left = 1;

        // make a copy once; normalize only for comparisons
        const std::string expected_norm = [&]{
            std::string t = expected_raw; normalize(t); return t;
        }();

        while (!matched && ok) {
            std::unique_lock<std::mutex> lk(g.m);
            unsigned long long my_ticket = g.next_ticket++;

            // Wait for our turn (with cancel/timeout)
            if (!g.cv.wait_until(lk, deadline, [&]{ return g.serving == my_ticket || (cancel && cancel->load()); })) {
                ok = false; break; // timed out before our turn
            }
            if (cancel && cancel->load()) { ok = false; break; }
            if (g.serving != my_ticket)   { ok = false; break; } // double-check

            // ---- purge expired saved (under lock) ----
            while (!g.pending.empty() && g.pending.front().expire_at <= g.serving) {
                g.pending.pop_front();
            }

            // ---- try to consume any saved command that matches our expected ----
            if (!g.pending.empty()) {
                auto it = std::find_if(g.pending.begin(), g.pending.end(),
                    [&](const Saved& s){ return s.expire_at > g.serving && match_normalized(s.text, expected_norm); });
                if (it != g.pending.end()) {
                    matched = true;
                    g.pending.erase(it);
                    ++g.serving;
                    lk.unlock();
                    g.cv.notify_one();
                    break; // done, no I/O
                }
            }

            // ---- no saved match: if others are queued AND there are saved lines, yield once ----
            if (!g.pending.empty() && g.next_ticket > g.serving + 1 && yields_left > 0) {
                --yields_left;
                my_ticket = g.next_ticket++;   // requeue ourselves at back
                // loop and wait again with the SAME deadline
                continue;
            }

            // ---- fall back to real input ----
            lk.unlock();
            std::cerr << "\n" << prompt << "\n" << std::flush;

            std::string line;
            if (!std::getline(std::cin, line)) {
                ok = false;
            } else {
                matched = match_normalized(line, expected_norm);
            }

            // save non-matching real input for the next N turns
            lk.lock();
            if (ok && !matched && pass_limit > 0) {
                g.pending.push_back(Saved{
                    std::move(line),
                    g.serving + static_cast<unsigned long long>(pass_limit)
                });
            }

            ++g.serving;
            lk.unlock();
            g.cv.notify_one();
        }

        return matched && ok;
    }

    // ---- queue mgmt: preload / push / reset / introspection ----

    inline void reset(bool clear_pending = true) {
        std::lock_guard<std::mutex> lk(g.m);
        g.next_ticket = g.serving = 0;
        if (clear_pending) g.pending.clear();
    }

    // push one saved line; offered to the next `pass_limit` turns
    inline void push_saved(std::string line, unsigned int pass_limit = 3) {
        std::lock_guard<std::mutex> lk(g.m);
        normalize(line);
        g.pending.push_back(Saved{
            std::move(line),
            g.serving + static_cast<unsigned long long>(pass_limit)
        });
    }

    // preload many; pass_limit < 0 means "infinite" (until matched)
    inline void preload(std::initializer_list<std::string> lines,
                        int pass_limit = -1,
                        bool clear_existing = true) {
        std::lock_guard<std::mutex> lk(g.m);
        if (clear_existing) g.pending.clear();
        const auto inf = (std::numeric_limits<unsigned long long>::max)(); // dodge Windows max macro
        for (auto& s : lines) {
            std::string t(s); normalize(t);
            g.pending.push_back(Saved{
                std::move(t),
                (pass_limit < 0) ? inf : (g.serving + static_cast<unsigned long long>(pass_limit))
            });
        }
    }

    template<class It>
    inline void preload(It first, It last,
                        int pass_limit = 3,
                        bool clear_existing = true) {
        std::lock_guard<std::mutex> lk(g.m);
        if (clear_existing) g.pending.clear();
        for (; first != last; ++first) {
            std::string t = *first; normalize(t);
            g.pending.push_back(Saved{
                std::move(t),
                g.serving + static_cast<unsigned long long>(pass_limit)
            });
        }
    }

    inline void preload_with_limits(std::initializer_list<std::pair<std::string,int>> items,
                                    bool clear_existing = true) {
        std::lock_guard<std::mutex> lk(g.m);
        if (clear_existing) g.pending.clear();
        const auto inf = (std::numeric_limits<unsigned long long>::max)();
        for (auto& [s, tries] : items) {
            std::string t(s); normalize(t);
            g.pending.push_back(Saved{
                std::move(t),
                (tries < 0) ? inf : (g.serving + static_cast<unsigned long long>(tries))
            });
        }
    }

    inline size_t pending_size() {
        std::lock_guard<std::mutex> lk(g.m);
        return g.pending.size();
    }

} // namespace stdin_fair
