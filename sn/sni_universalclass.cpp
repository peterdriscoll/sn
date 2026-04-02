#include "sni_universalclass.h"
#include "sn_pch.h"

namespace SNI
{
    SNI_UniversalClass::SNI_UniversalClass()
        : SNI_Class("All", "SN") // Or whatever "Root" name you prefer
    {
    }

    SNI_UniversalClass::~SNI_UniversalClass()
    {
    }
    
    std::string SNI_UniversalClass::GetTypeName() const
    { 
        return "UniversalClass";
    }

    SN::SN_Value SNI_UniversalClass::DoIsA(const SNI_Value* p_Parent) const
    {
        // 1. Identity check: Is the target also the Universal Class?
        // This is the only way a Universal class returns True for IsA.
        if (this == p_Parent)
        {
            return skynet::True;
        }

        // 2. Otherwise, the Universal Set is NOT a subset of Short, Long, etc.
        // This is the "False" you need to keep the loop spinning in ConstructionTree.
        return skynet::False;
    }
}