#pragma once

#include <cstddef>   // offsetof
#include <new>       // std::launder

template <typename Facade, typename Data>
struct Pair {
    Facade f;
    Data   d;
};

// compile-time byte offset from f -> d inside Pair<F, D>
template <typename F, typename D>
inline constexpr std::size_t kOffF2D =
offsetof(Pair<F, D>, d) - offsetof(Pair<F, D>, f);

// sanity: D will be correctly aligned at that offset
template <typename F, typename D>
inline constexpr bool kOffAligned = (kOffF2D<F, D> % alignof(D)) == 0;

// hop helper: Facade* -> Data*
template <typename F, typename D>
inline D* data_from(F* f) noexcept {
    static_assert(kOffAligned<F, D>, "Data not aligned at computed offset");
    auto* base = reinterpret_cast<unsigned char*>(f);
    return std::launder(reinterpret_cast<D*>(base + kOffF2D<F, D>));
}

template <typename F, typename D>
inline const D* data_from(const F* f) noexcept {
    static_assert(kOffAligned<F, D>, "Data not aligned at computed offset");
    auto* base = reinterpret_cast<const unsigned char*>(f);
    return std::launder(reinterpret_cast<const D*>(base + kOffF2D<F, D>));
}
