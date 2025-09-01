#pragma once

#include "ref_advanced.h"

#include <utility>
#include <type_traits>

namespace PGC {

    // DirectA<T> stores T inline (no transactions, no promotions).
    // It mimics the minimal RefA<T> surface your code uses: operator->, operator*, Get(), bool/empty,
    // plus no-op PromoteNow/RequestPromotion so call-sites compile identically.
    //
    // Intentionally IMMOVABLE and NONCOPYABLE to enforce “unmovable cake”.

    template<class T>
    class DirectA {
    public:
        using Data = T;

        DirectA() = default;

        // No copying
        DirectA(const DirectA&) = delete;
        DirectA& operator=(const DirectA&) = delete;

        // Yes moving (lets containing types default their move)
        DirectA(DirectA&&) noexcept = default;
        DirectA& operator=(DirectA&&) noexcept = default;

        // Pointer ergonomics (match RefA usage like: m_self->m_Name; *m_self; etc.)
        T* Get()       noexcept { return std::addressof(value_); }
        const T* Get() const noexcept { return std::addressof(value_); }

        T* operator->()       noexcept { return std::addressof(value_); }
        const T* operator->() const noexcept { return std::addressof(value_); }

        T& operator*()        noexcept { return value_; }
        const T& operator*()  const noexcept { return value_; }

        // Convenience (not strictly required)
        T& value()       noexcept { return value_; }
        const T& value() const noexcept { return value_; }

        // API parity with RefA (no-ops in ST mode)
        void PromoteNow() noexcept {}
        void RequestPromotion(void* /*ignored*/ = nullptr) noexcept {}

        // Emptiness / truthiness (DirectA is always “bound”)
        bool empty() const noexcept { return false; }
        explicit operator bool() const noexcept { return true; }

    private:
        T value_{};   // inline, fixed address for life of DirectA
    };

// Build-time switch:
//   - Define PGC_SINGLE_THREADED for “unmovable inline cake” (DirectA<T>).
//   - Else default to RefA<T> (multi-threaded movable cake).
#define PGC_SINGLE_THREADED
#ifdef PGC_SINGLE_THREADED
    template<class T> using SelfA = DirectA<T>;
    inline constexpr bool kFacadeProxyEnabled = false; // optional global: proxy off in ST
#else
    template<class T> using SelfA = RefA<T>;
    inline constexpr bool kFacadeProxyEnabled = true;  // proxy allowed in MT
#endif
} // namespace PGC
