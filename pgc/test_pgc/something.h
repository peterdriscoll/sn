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

#include "dori.h"

#include <string>
#include <iostream>
#include <new>
#include <type_traits>
#include "command_input3.h"

// Relocatable Member Dispatch (DORI)
// A stable handle (control cell) owns type/txn metadata and DISPATCHES member calls
// to the object's current final copy, even when the instance relocates (promotion/copy).
// Not remote; not link-time relocation.
//
// Invariants:
//  1) clone -> SetTransaction -> captured promotions -> PromoteMembers
//  2) commit strategy (rebind OR set final) → publish LAST
//  3) live objects always have a non-null transaction
//  4) accounting happens exactly once (authoritative sweeper)
//
// Search keywords (for future readers):
//  handle–body, handle–payload, relocatable handles (Mac/Win), forwarding pointer, promotion

namespace DORI 
{
    class Something : public PGC::PGC_Base
    {
    public:
        struct Data /* non-virtual body */ {
            // payload
            std::string          m_Name;
            std::string          m_Description;
            PGC::RefA<Something> m_Next;   // links are via Facade handles
			bool				 m_Stop{ false };

            // convenience: trivial copy/move
            Data(const Data&) = default;
            Data& operator=(const Data&) = default;
            Data(Data&&) noexcept = default;
            Data& operator=(Data&&) noexcept = default;
            Data(std::string p_Name, std::string p_Description)
                : m_Name(std::move(p_Name))
                , m_Description(std::move(p_Description))
                , m_Next{}
            { }
            Data(std::string p_Name, std::string p_Description, PGC::RefA<Something> p_Next)
                : m_Name(std::move(p_Name))
                , m_Description(std::move(p_Description))
                , m_Next(std::move(p_Next))
            { }
            Data(bool p_Stop)
                : m_Stop(p_Stop)
            { }
        };

		// Macro implements RetrieveDescriptor() for fused Pair<Something,Data>
        //PGC_FUSED_CLASS(Something, Data);
        void RetrieveDescriptor(char*& p, long& s) const override 
        {
            p = const_cast<char*>(reinterpret_cast<const char*>(this));
            s = static_cast<long>(sizeof(Pair<Something, Data>));
        }

    private:
        // Self points at the co-allocated Data when fused, or in ST may inline it.
        DORI::Self<Something, Data> m_self;

    public:
        // --- lifecycle (keep minimal) ---
        Something() = default;
        Something(const Something&) = default;
        Something(PGC::PGC_Transaction& txn, PGC::NoAutoReg noAuto, PGC::FusedTag)
            : PGC::PGC_Base(txn, noAuto)
            , m_self(DirectA<Data>{ DORI::to_data_ptr<Something, Data>(this) })  // bind to co-allocated Data
        {
        }
        Something(PGC::PGC_Transaction& txn, PGC::FusedTag)
            : PGC::PGC_Base(txn)
            , m_self(DirectA<Data>{ DORI::to_data_ptr<Something, Data>(this) })  // bind to co-allocated Data
        {
        }
        Something(PGC::RefA<Something>& p_ref, PGC::StackAllocationTag) noexcept
            : PGC::PGC_Base(PGC::StackAllocationTag{})
            , m_self(p_ref)
        {}

        void Finalize() noexcept override
        {
            //std::cout << "Finalize Something " << (m_self ? m_self->m_Name : "<null>") << "\n";
            m_self->m_Next.Finalize();
            m_self.Finalize();
        }
    Something& operator=(const Something&) = default;
        Something(Something&&) noexcept = default;
        Something& operator=(Something&&) noexcept = default;

        // Create JSON from the Data reachable via SelfA.
        void MakeJSON(std::ostream& os) const {
            stdin_fair::WaitForCommandScript("Type 'go' to process " + m_self->m_Name, "go");
            os << "{ \"name\":\"" << m_self->m_Name
                << "\", \"description\":\"" << m_self->m_Description << "\"";

            if (!m_self->m_Stop && m_self->m_Next) {
                os << ", \"next\": ";
                auto next = m_self->m_Next;
                next->MakeJSON(os);
            }
            os << " }";
        }

        PGC::RefA<Something> GetNext()
        {
            return m_self->m_Next;
        }
        void SetNext(PGC::RefA<Something> next) 
        {
            m_self->m_Next = std::move(next);
		}

        void Stop()
        {
            m_self->m_Stop = true;
        }

        // Expose if façade code needs the data view
        DORI::Self<Something, Data>& Self()
        {
            return m_self; 
        }
        const DORI::Self<Something, Data>& Self() const
        {
            return m_self; 
        }

        // --- MoveTo: build a fused Pair<Something,Data> in 'memory' and return new facade ---
        // Allocator must provide storage with alignment >= alignof(Pair<Something,Data>).
        virtual PGC::PGC_Base* MoveTo(void* memory) override
        {
            using PairT = DORI::Pair<Something, Data>;
            auto* pair = static_cast<PairT*>(memory);

            // 1) new facade (txn will be set by the caller per your PGC flow)
            ::new (&pair->f) Something(*PGC::PGC_Transaction::TopTransaction(), PGC::FusedTag{});

            // 2) new payload (move from old payload)
            ::new (&pair->d) Data(std::move(*m_self));

            return &pair->f;
        }
    };

} // namespace DORI
