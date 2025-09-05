#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <algorithm>

namespace script {

    // ---- global state ---------------------------------------------------
    static std::mutex              m;
    static std::condition_variable cv;
    static std::vector<std::string> cmds;
    static std::size_t             idx = 0;

    // Load/replace the script (resets position to 0)
    inline void Load(std::initializer_list<std::string> s) {
        std::lock_guard<std::mutex> lk(m);
        cmds.assign(s.begin(), s.end());
        idx = 0;
        cv.notify_all();
    }

    // One wait function:
    // - Blocks until the current token matches `expected`, OR
    // - If the current token matches a handler key, it consumes it, runs the handler, and keeps waiting
    // - The script advances only when something is accepted (expected or a handler)
    // - Returns false iff script is exhausted
    using Handler = std::pair<std::string, std::function<void()>>;
    inline bool Wait(const std::string& expected,
        std::initializer_list<Handler> handlers = {})
    {
        for (;;) {
            std::unique_lock<std::mutex> lk(m);

            auto ready = [&] {
                if (idx >= cmds.size()) return true; // EOF
                const std::string& t = cmds[idx];
                if (t == expected) return true;
                return std::any_of(handlers.begin(), handlers.end(),
                    [&](auto& h) { return t == h.first; });
                };

            cv.wait(lk, ready);                       // wait until head token is actionable
            if (idx >= cmds.size()) return false;     // no more input

            const std::string t = cmds[idx];

            // Accept expected token
            if (t == expected) {
                ++idx;
                lk.unlock();
                cv.notify_all();
                return true;
            }

            // Accept and run a side-command handler
            auto it = std::find_if(handlers.begin(), handlers.end(),
                [&](auto& h) { return t == h.first; });
            if (it != handlers.end()) {
                ++idx;
                auto fn = it->second;                 // run outside the lock
                lk.unlock();
                if (fn) fn();
                cv.notify_all();
                continue;                             // keep waiting for `expected`
            }

            // Shouldn't reach here due to predicate; loop guards us if spurious wake
        }
    }

} // namespace script
