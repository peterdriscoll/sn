// RefCore logical-rebind skeleton (single-thread minimal)
// ------------------------------------------------------
// This file is a self-contained skeleton showing the two cases:
//  1) Reuse existing promotion (same tracked slot)
//  2) Rebind promotion to a new logical slot (all-logical path)
//
// Assumptions / placeholders (replace with your real types):
//  - Base            : your promoted base type (PGC_Base or equivalent)
//  - PGC_Transaction : your transaction type
//  - PGC_Promotion   : your promotion tracker for a slot (m_Base**)
//
// Notes:
//  - Single-thread only (no atomics, no TLS guards here).
//  - No accounting is implemented; hooks are provided for you to wire up.
//  - Null-safe: a null logical slot is allowed; promotion becomes inert.
//
#pragma once

#include <utility>
#include <cstddef>

namespace PGCX { struct PGC_Transaction; }

struct Base {
    // Minimal interface expected by this skeleton.
    PGCX::PGC_Transaction* GetLogicalOwnerTransaction() const noexcept { return m_Owner; }
protected:
    PGCX::PGC_Transaction* m_Owner = nullptr; // for demo only
};

namespace PGCX {
struct PGC_Transaction { /* real implementation elsewhere */ };

struct PGC_Promotion {
    // Tracks a slot (m_Base**) and can promote when needed.
    void Disarm() noexcept { m_armed = false; }
    void Arm()   noexcept { m_armed = true; }
    bool IsArmed() const noexcept { return m_armed; }

    // Rebind the tracked slot; may be nullptr to disassociate.
    void RebindTo(Base** slot) noexcept { m_tracked = slot; }

    Base** TrackedSlot() const noexcept { return m_tracked; }

private:
    Base** m_tracked = nullptr;
    bool   m_armed   = false;
};
} // namespace PGCX

// Canonical logical pointer view: describes *where* a raw lives and *who* owns that slot
struct LogicalPtr {
    Base**                 slot = nullptr;   // address of the raw pointer being tracked
    PGCX::PGC_Transaction* owner = nullptr;  // owner of that slot (from source viewpoint)

    Base* raw() const noexcept { return slot ? *slot : nullptr; }
    explicit operator bool() const noexcept { return slot != nullptr; }
};

// A minimal RefCore that stores a logical pointer and a promotion handle
class RefCore {
public:
    RefCore() = default;

    // Construct from an existing logical pointer + destination owner txn
    RefCore(const LogicalPtr& lp, PGCX::PGC_Transaction* dest)
        : m_Pointer{ lp.slot, dest }
    {
        ensurePromotionBoundTo(lp.slot);
        if (m_Promotion) m_Promotion->Arm();
    }

    // Query our current logical pointer (slot + our destination owner)
    LogicalPtr GetLogicalPointer() const noexcept { return m_Pointer; }

    // Destination/owner txn accessor
    void SetDestinationTxn(PGCX::PGC_Transaction* dst) noexcept { m_Pointer.owner = dst; }
    PGCX::PGC_Transaction* GetDestinationTxn() const noexcept { return m_Pointer.owner; }

    // --- CASE 1: REUSE (same slot) is an automatic fast-path inside assignLogical() ---

    // --- CASE 2: REBIND (all logical) path ---
    // Assign from a *logical* input (already normalized) while keeping our destination txn.
    void AssignLogical(const LogicalPtr& in) {
        // 1) Disarm
        if (m_Promotion) m_Promotion->Disarm();

        // 2) Snapshot old state
        Base** oldSlot = m_Pointer.slot;
        Base*  oldRaw  = oldSlot ? *oldSlot : nullptr;
        PGCX::PGC_Transaction* dstTxn = m_Pointer.owner; // destination owner (ours)

        // 3) New slot from input
        Base** newSlot = in.slot; // input owner is descriptive-only here

        // 4) Reuse vs Rebind
        ensurePromotionBoundTo(newSlot);

        // 5) Commit our logical pointer (slot from input, owner = our destination txn)
        m_Pointer.slot  = newSlot;
        m_Pointer.owner = dstTxn;

        // 6) Re-arm
        if (m_Promotion) m_Promotion->Arm();

        // 7) Optional bookkeeping hooks
        if (oldSlot != newSlot) OnSlotRebound(oldSlot, newSlot);
        Base* newRaw = newSlot ? *newSlot : nullptr;
        if (oldRaw != newRaw)  OnRawChanged(oldRaw, newRaw, dstTxn);
    }

    // Assign from another RefCore (pull its logical pointer), keeping our destination
    void AssignFromRef(const RefCore& other) {
        AssignLogical(other.GetLogicalPointer());
    }

    // Assign from a raw pointer by *lifting* to a logical view
    void AssignFromRaw(Base* raw, Base** rawSlotAddr /*address of the raw*/ ) {
        LogicalPtr in{ rawSlotAddr, raw ? raw->GetLogicalOwnerTransaction() : nullptr };
        AssignLogical(in);
    }

    // Utility: tell whether we are currently tracking the same slot as another RefCore
    bool TracksSameSlotAs(const RefCore& other) const noexcept {
        return m_Pointer.slot == other.m_Pointer.slot;
    }

private:
    void ensurePromotionBoundTo(Base** targetSlot) {
        if (!m_Promotion) m_Promotion = makePromotion();
        if (m_Promotion->TrackedSlot() != targetSlot) {
            m_Promotion->RebindTo(targetSlot);
        }
    }

    static PGCX::PGC_Promotion* makePromotion() {
        // Replace with your allocation / factory
        static PGCX::PGC_Promotion dummy; // demo-only; likely you want per-instance
        return &dummy;
    }

    // Bookkeeping hooks (NO-OP here)
    void OnSlotRebound(Base** /*oldSlot*/, Base** /*newSlot*/) noexcept {}
    void OnRawChanged(Base* /*oldRaw*/, Base* /*newRaw*/, PGCX::PGC_Transaction* /*dstTxn*/) noexcept {}

private:
    LogicalPtr           m_Pointer{};   // (slot, destination-owner)
    PGCX::PGC_Promotion* m_Promotion{}; // tracks the slot (m_Base**)
};

// ---- Example wrapper Ref<T> (optional) ------------------------------------
// Shows how a typed Ref could delegate to RefCore while preserving T.
// In real code, Base would be a common base of T; adjust as needed.

template <class T>
class Ref {
public:
    Ref() = default;
    explicit Ref(const LogicalPtr& lp, PGCX::PGC_Transaction* dest) : core(lp, dest) {}

    LogicalPtr GetLogicalPointer() const noexcept { return core.GetLogicalPointer(); }

    void Assign(const Ref& r) { core.AssignFromRef(r.core); }

    void AssignRaw(T* raw, T** rawSlotAddr) {
        core.AssignFromRaw(static_cast<Base*>(raw), reinterpret_cast<Base**>(rawSlotAddr));
    }

    void SetDestinationTxn(PGCX::PGC_Transaction* dst) { core.SetDestinationTxn(dst); }
    PGCX::PGC_Transaction* GetDestinationTxn() const { return core.GetDestinationTxn(); }

private:
    RefCore core;
};

// End of file
