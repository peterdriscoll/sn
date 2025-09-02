#pragma once

#include <cstddef>   // offsetof
#include <new>       // std::launder
#include <utility>   // std::move

// 1) Layout used only for offsetof (must be standard-layout)
template<class F, class D>
struct Pair { F f; D d; };

// 2) Compile-time offset from Facade -> Data inside Pair<F,D>
template<class F, class D>
inline constexpr std::size_t kOffF2D =
offsetof(Pair<F, D>, d) - offsetof(Pair<F, D>, f);

// 3) Hop helper: Facade* -> Data* (used by the wrapper when it holds RefA<F>)
template<class F, class D>
inline D* to_data_ptr(F* f) noexcept {
    auto* base = reinterpret_cast<unsigned char*>(f);
    return std::launder(reinterpret_cast<D*>(base + kOffF2D<F, D>));
}

// ---- minimal stand-ins for your handles ----
template<class F> struct RefA { F* f{}; F* facade() const noexcept { return f; } };
template<class D> struct DirectA { D* d{}; D* get()    const noexcept { return d; } };

// 4) Self<F,D> — a tiny tagged union of RefA<F> or DirectA<D>
template<class F, class D>
class Self {
    enum class Kind { RefFacade, DirectData } kind_;
    union { RefA<F> ref_; DirectA<D> dir_; };

public:
    // Hold a RefA<F>
    explicit Self(RefA<F> r) : kind_(Kind::RefFacade), ref_(r) {}
    // Hold a DirectA<D>
    explicit Self(DirectA<D> d) : kind_(Kind::DirectData), dir_(d) {}

    // Accessors
    bool is_ref()   const noexcept { return kind_ == Kind::RefFacade; }
    F* facade()   const noexcept { return is_ref() ? ref_.facade() : nullptr; }
    D* data()     const noexcept {
        return is_ref() ? to_data_ptr<F, D>(ref_.facade()) : dir_.get();
    }

    // Uniform arrow to Data
    D* operator->() const noexcept { return data(); }
    D& operator* () const noexcept { return *data(); }
};
