#pragma once

// pgc_ready.h
#include "pgc_base.h"
#include <type_traits>
#include <concepts>

namespace PGC {

    template<class T>
    concept HasMoveTo =
        requires (T & t, void* mem) {
            { t.MoveTo(mem) } -> std::same_as<PGC::PGC_Base*>;   // adjust namespace if needed
    };

    template<class T>
    concept CapturesOnMove =
        requires { { T::pgc_captures_on_move } -> std::convertible_to<bool>; }
    && (bool)T::pgc_captures_on_move;

    template<class T>
    concept PGC_Ready =
        std::derived_from<T, PGC_Base>&&
        std::is_nothrow_move_constructible_v<T>&&
        HasMoveTo<T>;
//        && CapturesOnMove<T>;

} // namespace PGC
