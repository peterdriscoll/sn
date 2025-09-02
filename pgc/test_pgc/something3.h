#pragma once
// Single/Multi-thread ready facade using SelfA<Facade,Data> and a fused Pair layout.
// NOTE: this assumes self_tiny.h provides:
//   template<class F,class D> struct SelfA {
//       static SelfA FromFused(F* f, D* d);    // bind to co-allocated (Pair) F|D
//       D*  operator->() const;                // yields Data*
//       D&  operator* () const;
//       explicit operator bool() const;
//   };
// If your SelfA differs, adjust the two calls to FromFused() below.

#include "pgc.h"
#include "ref_advanced.h"
#include "self_tiny.h"
#include <string>
#include <iostream>
#include <new>
#include <type_traits>

namespace NewStuff {

    // Layout-only helper: F then D, living contiguously in the PGC block.
    template<class F, class D>
    struct Pair { F f; D d; };

    class Something : public PGC::PGC_Base
    {
        PGC_CLASS(Something);

    public:
        struct Data /* non-virtual body */ {
            // payload
            std::string          m_Name;
            std::string          m_Description;
            PGC::RefA<Something> m_Next;   // links are via Facade handles

            // convenience: trivial copy/move
            Data() = default;
            Data(const Data&) = default;
            Data& operator=(const Data&) = default;
            Data(Data&&) noexcept = default;
            Data& operator=(Data&&) noexcept = default;
        };

    private:
        // Self points at the co-allocated Data when fused, or in ST may inline it.
        PGC::SelfA<Something, Data> m_self;

    public:
        // --- lifecycle (keep minimal) ---
        Something() = default;
        Something(const Something&) = default;
        Something& operator=(const Something&) = default;
        Something(Something&&) noexcept = default;
        Something& operator=(Something&&) noexcept = default;

        // Create JSON from the Data reachable via SelfA.
        void MakeJSON(std::ostream& os) const {
            const auto& name = m_self->m_Name;
            const auto& desc = m_self->m_Description;

            os << "{ \"name\":\"" << name
               << "\", \"description\":\"" << desc << "\"";

            if (m_self->m_Next) {
                os << ", \"next\": ";
                m_self->m_Next->MakeJSON(os);
            }
            os << " }";
        }

        // Expose if façade code needs the data view
        PGC::SelfA<Something, Data>& Self()       { return m_self; }
        const PGC::SelfA<Something, Data>& Self() const { return m_self; }

        // --- MoveTo: build a fused Pair<Something,Data> in 'memory' and return new facade ---
        // Allocator must provide storage with alignment >= alignof(Pair<Something,Data>).
        virtual PGC::PGC_Base* MoveTo(void* memory) override
        {
            using PairT = Pair<Something, Data>;
            static_assert(std::is_standard_layout_v<PairT>, "Pair must be standard-layout");

            // 1) Construct an *empty* fused pair at destination.
            auto* pair = ::new (memory) PairT{};

            // 2) Bind the new facade's SelfA to the co-allocated Data.
            pair->f.m_self = PGC::SelfA<Something, Data>::FromFused(&pair->f, &pair->d);

            // 3) Copy payload from current Data into the new Data.
            //    (Small complication: if Data has non-trivial invariants, prefer an explicit copy function.)
            pair->d = *m_self;  // copies m_Name, m_Description, m_Next

            // 4) Return the new facade pointer (PGC will park token / rebind on next Get()).
            return &pair->f;
        }
    };

} // namespace NewStuff
