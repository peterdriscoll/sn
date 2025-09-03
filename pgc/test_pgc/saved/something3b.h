#pragma once
// Single-threaded variant: uses ref_direct.h (DirectA/SelfA) for inline storage


#include "pgc.h"
#include "ref_advanced.h"
#include "ref_direct.h"
#include <string>
#include <iostream>

namespace NewStuff {

    class Something : public PGC::PGC_Base
    {
        PGC_CLASS(Something);

    public:
        struct Data : public PGC::PGC_Base 
        {
            PGC_CLASS(Data);

            Data() = default;
            Data(const Data&) = default;
            Data& operator=(const Data&) = default;

            Data(Data&&) noexcept = default;
            Data& operator=(Data&&) noexcept = default;

            virtual PGC::PGC_Base* MoveTo(void* memory) override
            {
                return ::new (memory) Data(std::move(*this));
            }

            std::string    m_Name;
            std::string    m_Description;
            PGC::RefA<Something> m_Next;                     // links stay as façade handles
        };

    private:
        PGC::SelfA<Data> m_self;
        
#ifdef PGC_SINGLE_THREADED
        // ST: inline Data; nothing to allocate
        explicit Something() = default;

        // If you need a “forwarding” ctor in ST, make it a normal copy or delete it:
        explicit Something(Something& source) = default;   // or "= delete;"
#else
        // MT: allocate Data on heap via RefA<Data>
        explicit Something()
            : m_self(PGC::RefA<Data>(new Data()))
        {
        }

        // MT “forwarding” ctor keeps your original behavior
        explicit Something(PGC::RefA<Something>& source)
            : m_self(source->m_self)
        {
        }
#endif
    public:
        Something(const Something&) = default;
        Something& operator=(const Something&) = default;

        Something(Something&&) noexcept = default;
        Something& operator=(Something&&) noexcept = default;

        virtual PGC::PGC_Base* MoveTo(void* memory) override
        {
            return ::new (memory) Something(std::move(*this));
        }

        void MakeJSON(std::ostream& os) const {
            auto name = m_self->m_Name;            // short borrow ends at ';'
            auto desc = m_self->m_Description;     // short borrow ends at ';'

            os << "{ \"name\":\"" << name
                << "\", \"description\":\"" << desc << "\"";

            PGC::RefA<Something> next(m_self->m_Next);            // copy the RefA<Something>, release parent
            if (next) {                            // only 'next' is borrowed during call
                os << ", \"next\": ";
                next->MakeJSON(os);
            }
            os << " }";
        };

        // Optional: expose if façade code needs it
        PGC::SelfA<Data> &Self()
        { 
            return m_self; 
        }

        template<class> friend struct Proxy;       // lets Proxy build façades
        template<class U> friend class RefA;
    };

} // namespace NewStuff

