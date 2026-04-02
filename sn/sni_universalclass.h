#pragma once

#include "sni_class.h"

namespace SNI
{
    class SNI_UniversalClass : public SNI_Class
    {
    public:
        SNI_UniversalClass();
        virtual ~SNI_UniversalClass();

        virtual std::string GetTypeName() const;

        // The logic "Floor and Ceiling"
        virtual SN::SN_Value DoIsA(const SNI_Value* p_Parent) const override;
        
        // Metadata overrides
        
        // Ensure it doesn't try to use an inherit list it doesn't need
        virtual bool IsFixed() const { return true; }

    private:
        // Prevent accidental copies to keep the pointer comparison (this == instance) fast
        SNI_UniversalClass(const SNI_UniversalClass& p_Other) = delete;
    };
}