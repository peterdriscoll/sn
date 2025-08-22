// RefCore + Ref (bind-on-first-use, single-thread)
// -------------------------------------------------
// Goal: a "miracle class" that hides all promotion/slot/txn details.
// - Sticky destination transaction (captured on first use; no txn arg on set)
// - Clear split between reuse (same slot) and rebind (new slot)
// - Safe promotion lifecycle: disarm → snapshot → rebind → commit → arm
// - Null-safe and easy to read at call sites
//
// Replace stubs with your real types when integrating.

#pragma once
#include <cstddef>
#include <utility>
#include <cassert>

// ===== Replace with your actual forward decls / includes =====================
namespace PGCX { struct PGC_Transaction; }

struct Base {
    PGCX::PGC_Transaction* GetLogicalOwnerTransaction() const noexcept { return m_Owner; }
protected:
    PGCX::PGC_Transaction* m_Owner = nullptr; // demo-only placeholder
};

namespace PGCX {
struct PGC_Transaction { /* real implementation elsewhere */ };

struct PGC_Promotion {
    // Tracks a pointer slot (Base**) to update if/when promotion occurs
    void Disarm() noexcept { m_armed = false; }
    void Arm()   noexcept { m_armed = true; }
    bool IsArmed() const noexcept { return m_armed; }

    void RebindTo(Base** slot) noexcept { m_tracked = slot; }
    Base** TrackedSlot() const noexcept { return m_tracked; }

private:
    Base** m_tracked = nullptr;
    bool   m_armed   = false;
};
} // namespace PGCX

// Optional: central assertion macro (swap with your project's)
#ifndef SN_ASSERT
#  define SN_ASSERT(cond, msg) do { if (!(cond)) { assert(cond && msg); } } while(0)
#endif

// ===== Logical pointer view ==================================================
struct LogicalPtr {
    Base**                 slot  = nullptr;  // where the raw lives (external slot)
    PGCX::PGC_Transaction* owner = nullptr;  // owner of that slot (descriptive)

    Base* raw() const noexcept { return slot ? *slot : nullptr; }
    explicit operator bool() const noexcept { return slot != nullptr; }
};

// ===== RefCore ===============================================================
// Holds the hidden state: (slot we track) + (sticky destination txn)
// Outsiders only see a Ref-like interface via the typed wrapper below.
class RefCore {
public:
    using TxnProvider = PGCX::PGC_Transaction* (*)();

    RefCore() = default;
    ~RefCore() { if (m_OwnsPromo) delete m_Promotion; }

    // Configure how we get the current transaction on first bind (set once at startup).
    static void SetTxnProvider(TxnProvider p) noexcept { s_TxnProvider = p; }

    // Visible readback helpers (non-owning):
    LogicalPtr GetLogicalPointer() const noexcept { return m_Pointer; }
    Base*      GetRaw()            const noexcept { return m_Pointer.slot ? *m_Pointer.slot : nullptr; }

    // Sticky destination owner API (rarely used; usually auto-captured on first assignment)
    void RehomeTo(PGCX::PGC_Transaction* newDst) noexcept { m_Pointer.owner = newDst; }
    PGCX::PGC_Transaction* DestinationTxn() const noexcept { return m_Pointer.owner; }

    // Assign from another logical pointer (already normalized).
    // Captures destination txn on first use if not set.
    void AssignLogical(const LogicalPtr& in) {
        ensureStickyDestination();

        // 1) Disarm
        if (m_Promotion) m_Promotion->Disarm();

        // 2) Snapshot old state
        Base** oldSlot = m_Pointer.slot;
        Base*  oldRaw  = oldSlot ? *oldSlot : nullptr;
        PGCX::PGC_Transaction* dstTxn = m_Pointer.owner; // sticky destination

        // 3) Target slot from input (input owner is descriptive-only)
        Base** newSlot = in.slot;

        // 4) Reuse vs Rebind
        ensurePromotionBoundTo(newSlot);

        // 5) Commit our logical pointer (slot changes; owner stays sticky)
        m_Pointer.slot = newSlot;
        m_Pointer.owner = dstTxn;

        // 6) Re-arm
        if (m_Promotion) m_Promotion->Arm();

        // 7) Optional bookkeeping hooks
        if (oldSlot != newSlot) OnSlotRebound(oldSlot, newSlot);
        Base* newRaw = newSlot ? *newSlot : nullptr;
        if (oldRaw != newRaw)  OnRawChanged(oldRaw, newRaw, dstTxn);
    }

    // Assign from another RefCore
    void AssignFromRef(const RefCore& other) { AssignLogical(other.GetLogicalPointer()); }

    // Assign from a raw pointer by lifting to a logical view.
    // You must pass the address-of the raw slot (so we know where it lives).
    void AssignFromRaw(Base* raw, Base** rawSlotAddr) {
        LogicalPtr in{ rawSlotAddr, raw ? raw->GetLogicalOwnerTransaction() : nullptr };
        AssignLogical(in);
    }

    // Exceptional path: adopt an existing promotion tracker (e.g., when deliberately
    // reusing a project/promotion). Caller promises lifetime and correctness.
    void AdoptExistingPromotion(PGCX::PGC_Promotion* external) {
        if (m_Promotion && m_OwnsPromo) delete m_Promotion;
        m_Promotion  = external;
        m_OwnsPromo  = false; // we don't own it
    }

private:
    // Capture destination txn lazily on first use (bind-on-first-use policy)
    void ensureStickyDestination() {
        if (!m_Pointer.owner) {
            SN_ASSERT(s_TxnProvider && "TxnProvider not set for RefCore");
            auto* cur = s_TxnProvider ? s_TxnProvider() : nullptr;
            SN_ASSERT(cur && "No current transaction available at first bind");
            m_Pointer.owner = cur;
        }
    }

    void ensurePromotionBoundTo(Base** targetSlot) {
        if (!m_Promotion) {
            m_Promotion = new PGCX::PGC_Promotion();
            m_OwnsPromo = true;
        }
        if (m_Promotion->TrackedSlot() != targetSlot) {
            m_Promotion->RebindTo(targetSlot);
        }
    }

    // Bookkeeping hooks (override/extend when integrating)
    void OnSlotRebound(Base** /*oldSlot*/, Base** /*newSlot*/) noexcept {}
    void OnRawChanged(Base* /*oldRaw*/, Base* /*newRaw*/, PGCX::PGC_Transaction* /*dstTxn*/) noexcept {}

private:
    LogicalPtr            m_Pointer{};     // (tracked slot, sticky destination owner)
    PGCX::PGC_Promotion*  m_Promotion{};   // tracker for the slot
    bool                  m_OwnsPromo{};   // true if we allocated m_Promotion

    static TxnProvider    s_TxnProvider;   // how we get CurrentTransaction()
};

inline RefCore::TxnProvider RefCore::s_TxnProvider = nullptr;

// ===== Typed wrapper: Ref<T> =================================================
// Minimal, typed facade over RefCore. Keeps call sites clean and obvious.

template <class T>
class Ref {
public:
    Ref() = default;

    // Assign from another Ref<T>
    Ref& operator=(const Ref& other) { core.AssignFromRef(other.core); return *this; }

    // Assign from a raw pointer + its slot address (required for logical tracking)
    void Set(T* raw, T** rawSlotAddr) {
        core.AssignFromRaw(static_cast<Base*>(raw), reinterpret_cast<Base**>(rawSlotAddr));
    }

    // Read back (non-owning)
    T*   Get() const noexcept { return static_cast<T*>(core.GetRaw()); }
    bool IsNull() const noexcept { return core.GetRaw() == nullptr; }

    // Advanced hooks (rare):
    void RehomeTo(PGCX::PGC_Transaction* dst) { core.RehomeTo(dst); }

    // Expose logical pointer for low-level plumbing when absolutely necessary
    LogicalPtr GetLogicalPointer() const noexcept { return core.GetLogicalPointer(); }

    // Exceptional adoption (rare): reuse an existing promotion tracker
    void AdoptExistingPromotion(PGCX::PGC_Promotion* external) { core.AdoptExistingPromotion(external); }

private:
    RefCore core;
};

// ===== Usage notes ===========================================================
// 1) At program init, wire RefCore to your transaction getter:
//    RefCore::SetTxnProvider(&CurrentTransaction);
//
// 2) Typical call sites:
//    Ref<Foo> a, b;
//    a = b;                       // reuses or rebinds to b's slot; txn stays sticky
//    a.Set(ptr, &ptr);             // lift raw to logical using its slot address
//
// 3) No txn arg on set: destination transaction is captured on first use and sticks.
//
// 4) When integrating, replace Base / PGC_Promotion / PGC_Transaction with real types,
//    and wire OnSlotRebound / OnRawChanged to your accounting/GC logic.

// ============================================================================
// USER MUTEX VARIANT (preferred): serialize all updates on a shared user mutex
// ============================================================================
// Rationale:
//  - No per-object atomics; no published snapshots.
//  - All Set* calls take the user's mutex, perform disarm→compose→assert→arm.
//  - Reads optionally take the same mutex (simple & correct).
//  - Still uses bind-on-first-use for sticky destination txn (via TxnProvider) unless
//    you explicitly set m_Transaction.

#include <mutex>

struct PGC_User {
    std::mutex& Mutex() noexcept { return m; }
private:
    std::mutex m;
};

class RefCore_UserLocked {
public:
    using TxnProvider = PGCX::PGC_Transaction* (*)();

    explicit RefCore_UserLocked(PGC_User* u) : user(u) {}

    static void SetTxnProvider(TxnProvider p) noexcept { s_TxnProvider = p; }

    // ---- Reads (lock for simplicity) ----
    LogicalPtr Snapshot() const {
        std::lock_guard<std::mutex> lk(user->Mutex());
        return m_Pointer; // combined
    }
    Base* GetRaw() const {
        std::lock_guard<std::mutex> lk(user->Mutex());
        return m_Pointer.slot ? *m_Pointer.slot : nullptr;
    }

    // ---- Writes ----
    void SetArmed(bool on) {
        std::lock_guard<std::mutex> lk(user->Mutex());
        m_Armed = on;
        if (m_Promotion) (on ? m_Promotion->Arm() : m_Promotion->Disarm());
    }

    void SetPointerOrig(const LogicalPtr& in) {
        std::lock_guard<std::mutex> lk(user->Mutex());
        ensureStickyDestination();
        if (m_Promotion) m_Promotion->Disarm();
        const auto old = captureOld();
        m_PointerOrig = in;
        composeWriteAndAssert();
        if (m_Promotion && m_Armed) m_Promotion->Arm();
        postDelta(old);
    }

    void SetPromotion(PGCX::PGC_Promotion* p) {
        std::lock_guard<std::mutex> lk(user->Mutex());
        ensureStickyDestination();
        if (m_Promotion) m_Promotion->Disarm();
        const auto old = captureOld();
        m_Promotion = p; // caller may separately call p->RebindTo(...) if needed
        composeWriteAndAssert();
        if (m_Promotion && m_Armed) m_Promotion->Arm();
        postDelta(old);
    }

    void SetTxn(PGCX::PGC_Transaction* t) {
        std::lock_guard<std::mutex> lk(user->Mutex());
        m_Transaction = t;
        composeWriteAndAssert(); // owner must match txn; slot unchanged
    }

    // Convenience: assigning from Ref-like things
    void AssignFromRef(const LogicalPtr& otherLogical) { SetPointerOrig(otherLogical); }
    void AssignFromRaw(Base* raw, Base** rawSlotAddr) {
        LogicalPtr in{ rawSlotAddr, raw ? raw->GetLogicalOwnerTransaction() : nullptr };
        SetPointerOrig(in);
    }

    // Exceptional path: adopt an external promotion tracker (explicit and noisy)
    void AdoptExistingPromotion(PGCX::PGC_Promotion* external) {
        std::lock_guard<std::mutex> lk(user->Mutex());
        if (m_Promotion) m_Promotion->Disarm();
        const auto old = captureOld();
        m_Promotion = external; // no ownership logic here; caller manages lifetime
        composeWriteAndAssert();
        if (m_Promotion && m_Armed) m_Promotion->Arm();
        postDelta(old);
    }

private:
    struct OldSnapshot { Base** slot; Base* raw; };

    void ensureStickyDestination() {
        if (!m_Transaction && s_TxnProvider) {
            auto* cur = s_TxnProvider();
            SN_ASSERT(cur && "No CurrentTransaction available at first bind");
            m_Transaction = cur;
        }
    }

    LogicalPtr compose() const noexcept {
        LogicalPtr out;
        out.slot  = (m_Promotion && m_Promotion->TrackedSlot())
                  ?  m_Promotion->TrackedSlot()
                  :  m_PointerOrig.slot;
        out.owner = m_Transaction;
        return out;
    }

    void composeWriteAndAssert() {
        const LogicalPtr expected = compose();
        m_Pointer = expected; // single writer under lock
        assertInvariants(expected);
    }

    OldSnapshot captureOld() const noexcept {
        return { m_Pointer.slot, m_Pointer.slot ? *m_Pointer.slot : nullptr };
    }

    void postDelta(const OldSnapshot& old) {
        Base** newSlot = m_Pointer.slot;
        Base*  newRaw  = newSlot ? *newSlot : nullptr;
        if (old.slot != newSlot) OnSlotRebound(old.slot, newSlot);
        if (old.raw  != newRaw ) OnRawChanged(old.raw, newRaw, m_Pointer.owner);
    }

    void assertInvariants(const LogicalPtr& expected) const {
        SN_ASSERT(m_Pointer.slot  == expected.slot,  "slot mismatch");
        SN_ASSERT(m_Pointer.owner == expected.owner, "owner mismatch");
        if (m_Promotion && m_Promotion->TrackedSlot()) {
            SN_ASSERT(m_Promotion->TrackedSlot() == expected.slot, "promotion slot mismatch");
        }
        Base* expRaw = expected.slot ? *expected.slot : nullptr;
        Base* gotRaw = m_Pointer.raw();
        SN_ASSERT(expRaw == gotRaw, "raw mismatch");
    }

    // Hooks (no-op here)
    void OnSlotRebound(Base** /*oldSlot*/, Base** /*newSlot*/) noexcept {}
    void OnRawChanged(Base* /*oldRaw*/, Base* /*newRaw*/, PGCX::PGC_Transaction* /*dstTxn*/) noexcept {}

private:
    PGC_User*             user{};
    bool                  m_Armed{true};

    LogicalPtr            m_PointerOrig{};   // source logical view
    PGCX::PGC_Promotion*  m_Promotion{};     // slot tracker
    PGCX::PGC_Transaction* m_Transaction{};  // sticky destination

    LogicalPtr            m_Pointer{};       // derived combined view

    static TxnProvider    s_TxnProvider;
};

inline RefCore_UserLocked::TxnProvider RefCore_UserLocked::s_TxnProvider = nullptr;

// Typed facade using the user-locked core

template <class T>
class Ref_UL {
public:
    explicit Ref_UL(PGC_User* u) : core(u) {}

    void Assign(const Ref_UL& other) { core.AssignFromRef(other.core.Snapshot()); }
    void Set(T* raw, T** rawSlotAddr) {
        core.AssignFromRaw(static_cast<Base*>(raw), reinterpret_cast<Base**>(rawSlotAddr));
    }

    T*   Get()  const { return static_cast<T*>(core.GetRaw()); }
    bool IsNull() const { return core.GetRaw() == nullptr; }

    void SetArmed(bool on) { core.SetArmed(on); }
    void SetTxn(PGCX::PGC_Transaction* t) { core.SetTxn(t); }

    LogicalPtr GetLogicalPointer() const { return core.Snapshot(); }

private:
    RefCore_UserLocked core;
};

