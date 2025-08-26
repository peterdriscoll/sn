#pragma once

#include <typeindex>
#include "pgc_ready.h"  // your PGC_Ready checks

#define PGC_CLASS_ENTRY(T) ::PGC::make_entry<T>(#T),

#define PGC_CLASS_REGISTRY \
    { \
        PGC_CLASS_LIST(PGC_CLASS_ENTRY) \
        ::PGC::kEndSentinel \
    }

#define PGC_DEFINE_CLASS_REGISTRY(NAME) \
    const ::PGC::RegEntry NAME[] = PGC_CLASS_REGISTRY;

namespace PGC
{
    struct RegEntry {
        const char* name;
        std::type_index    type;  // stable per-type id (RTTI)
        // add more fields later (factories, vtables, etc.)
    };

    inline const RegEntry kEndSentinel
        { 
            /*name*/ nullptr,
            /*type*/ std::type_index(typeid(void))
        };

    template<class T>
    inline RegEntry make_entry(const char* name)
    {
        static_assert(std::derived_from<T, PGC::PGC_Base>,
            "PGC: T must publicly derive from PGC_Base");
        static_assert(std::is_nothrow_move_constructible_v<T>,
            "PGC: T must have a noexcept move constructor");
        static_assert(PGC::HasMoveTo<T>,
            "PGC: T must implement 'PGC_Base* MoveTo(void*)'");
        static_assert(PGC_Ready<T>,
            "PGC: Type is not PGC-ready (must derive PGC_Base, be nothrow-movable, "
            "and implement PGC_Base* MoveTo(void*) ).");
        return { /*name*/ name, /*type*/ std::type_index(typeid(T)) };
    };
} // namespace PGC
