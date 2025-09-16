#pragma once

//#include "debug.h"
#define PGC_FUSED_CLASS(F, D)                                      \
    void RetrieveDescriptor(char*& p, long& s) const override      \
    {                                                              \
        p = const_cast<char*>(reinterpret_cast<const char*>(this));\
        s = static_cast<long>(sizeof(Pair<F, D>));                 \
    }

namespace DORI
{
    // layout-only pair for offsetof
    template<class F, class D>
    struct Pair { F f; D d; };


    // compile-time offset F ? D

    //template<class F, class D>
    //inline constexpr std::size_t kOffF2D =
    //    offsetof(Pair<F, D>, d) - offsetof(Pair<F, D>, f);
    template<class F, class D>
    constexpr std::size_t kOffF2D() noexcept {
        using PairT = Pair<F, D>;                 // alias avoids template-arg commas in macro
        return offsetof(PairT, d) - offsetof(PairT, f);
    }

    // hop helper (all nastiness lives here)
    template<class F, class D>
    inline D* to_data_ptr(F* f) noexcept {
        auto* base = reinterpret_cast<unsigned char*>(f);
        return std::launder(reinterpret_cast<D*>(base + kOffF2D<F, D>()));
    }

    // minimal stand-ins for your handles
    template<class D> struct DirectA { D* d{}; };

    // Self = union of �facade ref� or �direct data�
    template<class F, class D>
    struct Self {
        bool via_facade;   // true ? use f; false ? use d
        PGC::RefA<F> rf;
        D* d;

        // constructors
        explicit Self(PGC::RefA<F>& r)
            : via_facade(true), rf(r)
        {
            //			rf.RequestPromotion(PGC::PGC_Transaction::TopTransaction());
        }
        explicit Self(DirectA<D> x) : via_facade(false), d(x.d) {}
        void Finalize() noexcept
        {
            rf.Finalize();
        }

        struct LockedPin {
            PGC::EnGuard lk;
            D* ptr;

            // acquire user lock, then resolve D*
            LockedPin(PGC_User& user, Self& self)
                : lk(user.Acquire())
                , ptr(nullptr)
            {
                ptr = self.via_facade
                    ? to_data_ptr<F, D>(self.rf.Get())   // recursive lock is OK
                    : self.d;
            }
            ~LockedPin()
            {
            };

            D* operator->() { return ptr; }
            const D* operator->() const { return ptr; }
            D& operator*() { return *ptr; }
            const D& operator*()  const { return *ptr; }
            explicit operator bool()const { return ptr != nullptr; }
        };

        LockedPin operator->() {
            auto* user = PGC_User::GetCurrentPGC_User();
            return LockedPin(*user, *this);
        }
        const LockedPin operator->() const {
            auto* user = PGC_User::GetCurrentPGC_User();
            return LockedPin(*user, const_cast<Self&>(*this));
        };

        // always give me D*
        const D* data() const noexcept { return via_facade ? to_data_ptr<F, D>(rf.Get()) : d; };
        D* data() noexcept { return via_facade ? to_data_ptr<F, D>(rf.Get()) : d; };

        const D& operator* () const noexcept { return *data(); }
        D& operator* () noexcept { return *data(); }
        // convenience
    };
}

