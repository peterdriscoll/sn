# PGC Threading Design

> **Purpose**: Define a safe, simple threading model for PGC (Promotion-aware, arena-based memory) that preserves correctness by default, keeps the fast path fast, and scales from single-threaded today to multi-threaded tomorrow with minimal churn.

---

## Scope & Non‑Goals

**In scope**
- Read & write access to objects owned by transactions from multiple threads.
- Ending (tearing down) transactions while other threads are active.
- Publication rules for pointers vs. references.
- Optional, zero-risk scaffolding that can be kept **off by default** today.

**Out of scope**
- Complex lock hierarchies beyond transaction-local locks already used by promotions.
- Distributed/shared-memory designs; this document targets a single process.

---

## Mental Model (Recap)

- A **Transaction (Txn)** owns an **arena** and all objects allocated within it.
- On **end-of-transaction**: (1) **promotions finalize first**, (2) **unpromoted** objects are destroyed, (3) arena memory is freed.
- **Promotion** copies/moves an object from a **source** txn to a **destination** txn when a cross-txn reference exists.
- **Strategies**:
  - **Double‑Dipping (DD)** *(default)*: a small indirection that can **rebind** to a new holder slot on moves; one extra read indirection.
  - **Backstabbing** *(advanced / optional)*: record a `T**` and rewrite it at finalize; only safe if the slot never moves or you implement rebinding.

---

## Safety Invariants

1. **No use-after-free**: No thread may dereference an object from a txn while that txn is being torn down.
2. **Promotions-before-free**: At end-of-txn, promotions are processed *before* memory is reclaimed.
3. **No raw publication**: Never publish raw `T*` across transactions/threads; published data must be via **Ref/MemberRef** that register promotions.
4. **Move-safety**: Any ref holder that can relocate (e.g., `std::vector`) must use DD or explicitly rebind backstab requests when moved.
5. **Idempotent ref dtors**: Destructors of ref types never dereference raw pointers; they act only on known promotions.

---

## Publication Rules (Pointers vs. Refs)

- **Raw `T*`**: allowed only for local, same-txn, same-thread, short-lived reads; must **not** escape the txn boundary.
- **`MemberRef<T>` / `Ref<T>`**: used for any value that may cross a txn boundary or be observed by another thread. Cross-txn assignment registers a promotion; DD is the default strategy.
- **`SRef<T>` (optional)**: a short-lived, read-only **borrow** type that can auto‑pin a txn in threaded builds (see below). In non-threaded builds it aliases to `const T*`.

---

## Transaction End Sequence (Thread-Aware)

1) **Admission control** *(optional pinning; see below)*: stop new transient borrows if pinning is enabled.
2) **Finalize promotions** from inner/source txns into this txn.
3) **Destroy unpromoted** arena objects (run destructors).
4) **Free blocks** and release the arena.

> Holders of `MemberRef/Ref` should be destroyed **before** their owning txn ends (member declaration order or explicit `clear()` in owner destructors).

---

## Option A — Per‑Transaction Pins (Recommended When Threads Arrive)

**Goal**: Allow transient raw reads across threads *without* racing txn teardown, while minimizing global contention.

### Design

Each txn tracks `{ state ∈ {Open, Closing, Closed}, inbound_pins }`.

- **Pin (RAII)**: A tiny token taken from the **source** txn right before a borrow; increments `inbound_pins` and releases on destruction.
- **try_pin()**: Fast‑path borrow. Fails if the txn is `Closing`.
- **request_close_and_wait()**: Owner flips `state` to `Closing`, rejects new borrows, waits for `inbound_pins == 0`, then runs the normal teardown.

### API Sketch

```cpp
enum class TxnState { Open, Closing, Closed };

class PGC_Transaction {
public:
  class Pin {
  public:
    Pin() = default;
    explicit Pin(PGC_Transaction* t) : tx(t) {}
    Pin(Pin&& o) noexcept : tx(std::exchange(o.tx,nullptr)) {}
    Pin& operator=(Pin&& o) noexcept { if (this!=&o){ release(); tx=std::exchange(o.tx,nullptr);} return *this; }
    ~Pin() { release(); }
    explicit operator bool() const { return tx != nullptr; }
  private:
    void release();
    PGC_Transaction* tx = nullptr;
  };

  Pin try_pin();                 // returns empty pin if Closing/Closed
  void request_close_and_wait(); // flips to Closing; waits for pins==0; then caller tears down

private:
  std::atomic<TxnState> state{TxnState::Open};
  std::atomic<int>      inbound_pins{0};
  std::mutex            mu; std::condition_variable cv;
};
```

### Borrow Ergonomics

Hide pins behind `SRef<T>` so call‑sites don’t change:

```cpp
#if PGC_SREF_GUARD==0
  template<class T> using SRef = const T*; // alias until threads
#else
  template<class T>
  class SRef {
    const T* p_{}; PGC_Transaction::Pin pin_{};
  public:
    SRef() = default;
    /*implicit*/ SRef(const T* p) : p_(p), pin_(p ? p->Txn()->try_pin() : PGC_Transaction::Pin{}) {}
    const T* get() const { return p_; }
    const T* operator->() const { return p_; }
    const T& operator*() const { return *p_; }
    explicit operator bool() const { return p_ != nullptr; }
  };
#endif
```

`MemberRef<T>` can expose a non‑destructive `sref()` and an optional `operator->()` proxy that borrows for a single expression.

### Guidance

- Keep borrow scopes tight (don’t hold pins across I/O/locks).
- The closer must not hold a pin to the same txn.
- This is per‑txn: unrelated txns can close without blocking each other.

---

## Option B — Global Close Barrier (Simpler, More Intrusive)

A single **global** barrier pauses new transient borrows while a batch of txns closes.

- Threads take a **global pin** (increments a global reader count) for each transient borrow.
- First closer becomes **leader**: flips barrier on, waits for readers to drain, closes the queued txns one‑by‑one, then lifts the barrier.

**Use when**: closes are infrequent and you prefer trivial coordination to per‑txn pins.

---

## Promotions & Concurrency

- **Registration**: Cross‑txn assignment takes a lock on the **destination** txn while registering a promotion (unchanged).
- **Finalize**: Promotions are processed at **source** txn teardown, before any memory is reclaimed.
- **Double‑Dipping**: On move, holders must call `RebindBaseAddress(&slot)` (hidden in move ctor/assign of `MemberRef/Ref`).
- **Backstabbing** (if enabled): Use only for stable slots, or implement a slower rebind‑on‑move (e.g., source txn scans pending backstabs and updates any whose `base == old_slot`). Default remains **DD**.

---

## Delayed Evaluation (“Freeze for Delay”)

When enqueuing delayed work that stores `SN_Expression` values which **wrap `SNI_Expression*`**:

- Immediately **promote** the wrapped pointers into a stable arena (mutate the internal pointer in place).
- Promote other members that will be dereferenced later (e.g., `m_Source`, `m_Frame`).
- If errors carry `StringRef`, add `SN_Error::Owned()` and copy once at enqueue/return.

This keeps fixed‑size arrays lean but safe.

---

## Build Toggles (for gradual rollout)

```cpp
// Backstabbing default: off (use Double‑Dipping)
#ifndef PGC_ENABLE_BACKSTAB
#define PGC_ENABLE_BACKSTAB 0
#endif

// SRef auto‑pinning: off until threads arrive
#ifndef PGC_SREF_GUARD
#define PGC_SREF_GUARD 0
#endif

// Optional: debug accounting/asserts at txn end
#ifndef PGC_DEBUG_LEDGER
#define PGC_DEBUG_LEDGER 1
#endif
```

---

## Diagnostics & Testing

- **ASan** (or similar) to detect UAF quickly.
- **Debug ledger**: only `Block::{Allocate,DestroyUncopied}` mutate block bytes; only `Promotion::{Ctor,Free/Mark…}` mutate promotion bytes; both must be zero at txn end.
- **Move storm**: refs in a vector with forced reallocations; end inner txn → all refs valid.
- **Delayed call**: enqueue → free source blocks → evaluate → no dangling reads.
- **Teardown order**: destroy holders first; then txn; verify no dtors touch freed memory.
- **Threaded (when enabled)**: loop reads via `SRef` on thread A while ending txn on thread B; teardown waits for pins to drain.

---

## SN Wrapper Classes as Central Borrow Point

Many call sites already use the `SN::*` wrapper types (e.g., `SN_Expression`, `SN_Function`, `SN_Error`) rather than raw `SNI_*` pointers. We can leverage this to introduce safe borrows **in one place** without changing most code.

### Design
- **Do NOT store `SRef` inside the wrappers.** `SRef` is a *borrow* (short-lived pin). Storing it would hold a pin for too long.
- Keep today’s storage (raw pointer or `MemberRef`), but make the **accessors** return a borrow:
  - `operator->()` returns a **BorrowProxy** that holds an `SRef<const Impl>`; pin lasts only for that *full expression*.
  - `sref()` returns an `SRef<const Impl>` for multi-statement reads in a small scope.
- Backwards compatibility: retain `GetSNI_*()` returning a raw pointer for legacy code (use sparingly).

### Toggleable (no behavior change until you want it)
```cpp
// config.h
#ifndef PGC_SREF_GUARD
#define PGC_SREF_GUARD 0   // 0 = off (SRef is const T*), 1 = on (auto-pin)
#endif
```

### Helper alias
```cpp
#if PGC_SREF_GUARD==0
  template<class T> using SNBorrow = const T*;    // no-op today
#else
  template<class T> using SNBorrow = SRef<const T>; // auto-pins when enabled
#endif
```

### Wrapper pattern (example: SN_Expression)
```cpp
class SN_Expression {
  SNI_Expression* ptr_ = nullptr;   // or MemberRef<SNI_Expression> if you prefer
public:
  SN_Expression() = default;
  explicit SN_Expression(SNI_Expression* p) : ptr_(p) {}

  // One-shot borrow for `expr->Method()`
  struct BorrowProxy {
    SNBorrow<SNI_Expression> b;
    const SNI_Expression* operator->() const { return b; }
  };
  BorrowProxy operator->() const { return BorrowProxy{ SNBorrow<SNI_Expression>(ptr_) };
  }

  // Multi-statement read: pin for this scope only
  SNBorrow<SNI_Expression> sref() const { return SNBorrow<SNI_Expression>(ptr_); }

  // Legacy/raw escape hatch (prefer sref())
  SNI_Expression* GetSNI_Expression() const { return ptr_; }
  SNI_Expression*& GetSNI_ExpressionRef() { return ptr_; }
};
```

> Apply the same pattern to other SN wrappers (e.g., `SN_Function`, `SN_Error`, etc.). For wrappers with mutation APIs, keep mutation routed through `MemberRef`/promotion-aware paths and reserve `SRef`-based accessors for **read-only** operations.

### Benefits
- **Centralized safety**: flip `PGC_SREF_GUARD=1` and *all* wrapper-based reads gain pinning.
- **Zero churn**: existing `expr->Foo()` call sites keep working.
- **Short pins**: expression-scoped by default via the proxy; opt into slightly longer borrows with `sref()` where needed.

### Caveats
- Don’t take the address of a temporary borrow (e.g., `&*expr->`); if you need a pointer for longer, call `auto b = expr.sref();` and keep `b` inside a tight scope.
- Avoid holding a borrow across blocking I/O or locks.

---

## Migration Plan

1. **Today (single‑thread)**
   - Keep DD as default; backstab behind a flag.
   - Harden `MemberRef` dtors (idempotent; no raw deref). Ensure promotions disarm owner slots on `Free/Mark…`.
   - For delayed calls, promote members at enqueue.

2. **Add scaffolding (no behavior change)**
   - Introduce `SRef` behind `PGC_SREF_GUARD` (alias to `const T*`).
   - Add `MemberRef::sref()` and optional `operator->()` proxy.

3. **When threads arrive**
   - Implement per‑txn pins (`try_pin`, `request_close_and_wait`).
   - Flip `PGC_SREF_GUARD=1` so `SRef` auto‑pins behind the scenes.

---

## Appendix — RAII Pin internals (reference)

```cpp
inline PGC_Transaction::Pin PGC_Transaction::try_pin() {
  if (state.load(std::memory_order_acquire) != TxnState::Open) return {};
  inbound_pins.fetch_add(1, std::memory_order_acq_rel);
  if (state.load(std::memory_order_acquire) == TxnState::Open) return Pin{this};
  inbound_pins.fetch_sub(1, std::memory_order_acq_rel); // lost race with Closing
  return {};
}

inline void PGC_Transaction::Pin::release() {
  if (!tx) return;
  if (tx->inbound_pins.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    std::lock_guard<std::mutex> lk(tx->mu);
    tx->cv.notify_one();
  }
  tx = nullptr;
}

inline void PGC_Transaction::request_close_and_wait() {
  TxnState expected = TxnState::Open;
  state.compare_exchange_strong(expected, TxnState::Closing, std::memory_order_acq_rel);
  std::unique_lock<std::mutex> lk(mu);
  cv.wait(lk, [&]{ return inbound_pins.load(std::memory_order_acquire) == 0; });
  // caller: finalize promotions → destroy unpromoted → free arena → mark Closed
}
```

---

**End of document.**

