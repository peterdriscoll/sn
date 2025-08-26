#pragma once

#include "pgc_base.h"

#include <type_traits>
#include <concepts>

// One-line certification with targeted messages:
#define PGC_CERTIFY(T)                                                     \
  static_assert(std::derived_from<T, PGC::PGC_Base>,                       \
                "PGC: T must publicly derive from PGC_Base");              \
  static_assert(std::is_nothrow_move_constructible_v<T>,                   \
                "PGC: T must have a noexcept move constructor");           \
  static_assert(PGC::HasMoveTo<T>,                                         \
                "PGC: T must implement 'PGC_Base* MoveTo(void*)'");

namespace PGC 
{
    // --- Your base & detection bits (adjust names if needed) ---
    template<class T>
    concept HasMoveTo =
        requires (T & t, void* mem) {
            { t.MoveTo(mem) } -> std::same_as<PGC_Base*>;  // exact signature
    };

    // Optional flag you may re-enable later
    template<class T>
    concept CapturesOnMove =
        requires { { T::pgc_captures_on_move } -> std::convertible_to<bool>; }
    && (bool)T::pgc_captures_on_move;

    // Your “real” concept (use whatever you currently enforce)
    template<class T>
    concept PGC_Ready =
        std::derived_from<T, PGC_Base>&&
        std::is_nothrow_move_constructible_v<T>&&
        HasMoveTo<T>;
    //&& CapturesOnMove<T>;

    // --- EXPLAIN: emits targeted static_asserts with readable messages ---

    template<class T>
    struct ExplainPGCReady {
        static void check() {
            // Do these one-by-one so MSVC shows exactly which failed.
            static_assert(std::is_class_v<T>, "PGC: T must be a class type");

            static_assert(std::derived_from<T, PGC_Base>,
                "PGC: T must publicly derive from PGC_Base");

            static_assert(std::is_nothrow_move_constructible_v<T>,
                "PGC: T must have a noexcept move constructor "
                "(mark 'T(T&&) noexcept = default;' or ensure all bases/members are noexcept-movable)");

            static_assert(HasMoveTo<T>,
                "PGC: T must implement 'PGC_Base* MoveTo(void*)' with the exact signature");

            // Uncomment if/when you enforce the flag again
            // static_assert(CapturesOnMove<T>,
            //               "PGC: T must set 'static constexpr bool pgc_captures_on_move = true;'");
        }
    };
}