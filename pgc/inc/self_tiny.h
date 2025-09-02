#pragma once

// layout-only pair for offsetof
template<class F, class D>
struct Pair { F f; D d; };

// compile-time offset F ? D
template<class F, class D>
inline constexpr std::size_t kOffF2D =
offsetof(Pair<F, D>, d) - offsetof(Pair<F, D>, f);

// hop helper (all nastiness lives here)
template<class F, class D>
inline D* to_data_ptr(F* f) noexcept {
    auto* base = reinterpret_cast<unsigned char*>(f);
    return std::launder(reinterpret_cast<D*>(base + kOffF2D<F, D>));
}

// minimal stand-ins for your handles
template<class F> struct RefA { F* f{}; };
template<class D> struct DirectA { D* d{}; };

// Self = union of “facade ref” or “direct data”
template<class F, class D>
struct Self {
    bool via_facade;   // true ? use f; false ? use d
    union { F* f; D* d; };

    // constructors
    explicit Self(RefA<F> r) : via_facade(true), f(r.f) {}
    explicit Self(DirectA<D> x) : via_facade(false), d(x.d) {}

    // always give me D*
    D* data() const noexcept { return via_facade ? to_data_ptr<F, D>(f) : d; }

    // convenience
    D* operator->() const noexcept { return data(); }
    D& operator* () const noexcept { return *data(); }
};

