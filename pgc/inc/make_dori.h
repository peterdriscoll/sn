#pragma once

#include "dori.h"
#include <utility>

namespace DORI {
    template<class F> using FacadeDataT = typename F::Data;

    template<class F, class... DArgs>
    PGC::RefA<F> make_dori(PGC::PGC_Transaction& txn, DArgs&&... dargs) {
        using D = FacadeDataT<F>;
        using PairT = Pair<F, D>;

        // 1) allocate fused
        void* mem = txn.Allocate(sizeof(PairT), alignof(PairT));
        auto* pair = static_cast<PairT*>(mem);

        // 2) facade (no auto-reg)
        F* f = ::new (&pair->f) F(txn, PGC::NoAutoReg{}, PGC::FusedTag{});
        try 
        {
            ::new (&pair->d) D(std::forward<DArgs>(dargs)...);
        } 
        // 3) payload (can throw)
        catch (...) { f->~F(); throw; }

        txn.RegisterForDestruction(f);                               // 4) single registration
        return PGC::RefA<F>(f);                                           // 5) return handle
    }
}
