#pragma once

#include <cstddef>   // offsetof
#include <cstdint>
#include <cstring>
#include <new>
#include <type_traits>

// ---- Your concrete types (hard-coded for the demo) ----
struct Data {
    int value = 42;
    // ... no vtable here
};

struct Facade {
    // Facade has the vtable in your real code; not needed for the demo
    // (We won't put any virtuals here just to keep it tiny.)
};

// Lay the two side-by-side in a standard-layout wrapper.
// Only this wrapper needs to be standard-layout.
template <typename Facade, typename Data>
struct Pair {
    Facade f;
    Data   d;
};

// Compile-time byte offset from Facade to Data inside Pair.
static constexpr std::size_t kOff_F_to_D =
offsetof(Pair, d) - offsetof(Pair, f);

// Helper: compute Data* from Facade* using that offset.
inline Data* to_data_ptr(Facade* f) noexcept {
    auto* base = reinterpret_cast<unsigned char*>(f);
    return std::launder(reinterpret_cast<Data*>(base + kOff_F_to_D));
}

// Two simple “handles” with operator-> that yield Data*

struct RefA_Facade {
    Facade* f{};                // think: proxy to a Facade
    Data* operator->() const {  // arrow goes straight to Data
        return to_data_ptr(f);
    }
};

struct DirectA_Data {
    Data* d{};                  // direct pointer to Data
    Data* operator->() const { return d; }
};

// ---- minimal use ----
int main() {
    // Place a Pair somewhere (stack for the demo)
    alignas(Pair) unsigned char buf[sizeof(Pair)];
    auto* p = ::new (buf) Pair{};
    p->d.value = 123;

    RefA_Facade ref{ &p->f };
    DirectA_Data dir{ &p->d };

    int a = ref->value; // 123, via Facade -> Data hop
    int b = dir->value; // 123, direct

    (void)a; (void)b;
}
