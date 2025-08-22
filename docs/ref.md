# PGC Ref/Pin Overview

## TL;DR
- **`Ref<T>`** holds a pointer to `T` across transactions.  
  - Same‑txn ⇒ encodes a **direct pointer**.  
  - Cross‑txn ⇒ encodes a **Promotion** that will deliver a final copy at the destination txn via `Promotion::Rebind(&m_InternalPointer, dest)`.
- **Safe access**: `Ref<T>::operator->()` returns a **pin proxy** that pins the **source transaction** for the full expression, then unpins (RAII).
- **One mutex rule**: mutation paths lock **destination txn only**. Pins are **lock‑free** atomics.
- **Const reads don’t mutate.** Collapse/free happens in mutating paths (`Set/Clear/move/dtor`, optional non‑const `Get()` or explicit `Collapse()` when provided).

---

## Core Types

### `RefCore`
Encodes the internal pointer state:
- Direct `T*` when `source == destination`.
- `Promotion*` when `source != destination` (cross‑txn).

**Key APIs**
- `SetLogicalPointer(PGC_TypeCheck*)` — direct vs promotion depending on source/dest.
- `SetLogicalPromotion(PGC_Promotion*)` — calls `promotion.Rebind(&m_InternalPointer, dest)`.
- `DetachPromotion()` — returns the promotion without freeing it; resets encoding to final copy (if present) or owner‑txn sentinel.
- `Clear()` — frees any promotion (NQR list cleanup later) and resets to owner‑txn sentinel.
- `GetLogicalPointer() / GetLogicalPromotion() / GetLogicalOwnerTransaction()`.

### `Ref<T>`
Public façade over `RefCore`.

**Access operators (pinning)**
- `operator->()` / `operator*()` → return a **pin proxy**; *peek‑only* (no collapse).
- `Pinned()` → returns a proxy you can keep for multiple calls in one scope.

**Mutators**
- `Set(T*)` — under **dest lock**: `Clear()` then encode direct or create promotion.
- `Clear()` — under **dest lock**: frees promo (if any), reset to owner sentinel.
- `Get()` (non‑const, optional) — may **collapse‑if‑promoted** under dest lock and return the logical pointer.
- `Collapse()` (optional) — explicit collapse under dest lock.

**Moves / Dtor**
- Move‑ctor / move‑assign — adopt other’s pointer/promotion (rebind); free ours first under dest lock.
- Destructor — `Clear()` under dest lock.

### `PinProxy` (header‑only)
- Aliases: `Pin<T>` / `ConstPin<T>` implemented via a single template `PinProxy<Ptr>` where `Ptr` is `T*` or `const T*`.
- **Ctor**: cache `txn = ptr->GetTransaction()` (assert non‑null), then `txn->Pin()`.
- **Dtor**: `txn->Unpin()` — unpins the **same** txn that was pinned.
- **Operators**: `operator->()`, `operator*()` *(use `decltype(auto)`)*, `get()`, `explicit operator bool()`.
- Move‑only; dtor `noexcept`.

---

## Invariants (debug‑asserted)
- A `Ref` **always** has a non‑null **destination (owner) transaction**.
- Any `T*` used with `Set()` has a non‑null **source** transaction.
- `SetLogicalPromotion()` **rebinds base and destination**: `promotion.Rebind(&m_InternalPointer, dest)`.
- `Clear()` frees any promotion, then resets encoding to the **owner‑txn sentinel**.

---

## Access Semantics
**`ref->call()`**  
Returns a `Pin<T>` temporary. Execution order:
1. Proxy constructed → **pin source txn**.  
2. Proxy `operator->()` returns `T*`.  
3. `T::call(...)` executes.  
4. Proxy destroyed → **unpin** (even if an exception is thrown).

**Longer pin**
```cpp
auto pin = ref.Pinned();
pin->Step1();
pin->Step2();  // one pin for both calls
// pin goes out of scope ⇒ unpin
```

**Also pinned with `*`**
```cpp
(*ref)->Field = 42;  // operator* returns a proxy; then -> forwards
```

**Gotcha**  
Avoid splitting the temporary if you need the pin:
```cpp
auto* p = ref.operator->().get();  // pin ends at ';'
p->Use();                          // runs *unpinned*
```

---

## Mutation & Collapse
- Do **not** collapse in `operator->` / const reads. Keep them side‑effect‑free.
- Collapse & free promotions in:
  - `Set(T*)` (under **dest lock**): `Clear()` then encode direct or promotion.
  - Move‑assign/ctor (under **dest lock**): adopt via `DetachPromotion()` then `SetLogicalPromotion()`.
  - `Clear()` / destructor (under **dest lock**).
  - (Optional) **non‑const `Get()`** or explicit `Collapse()` to opportunistically tidy finished promotions.

---

## Synchronization Model (deadlock‑free)
- **Pins**: `Pin()`/`Unpin()` are **atomic** increments/decrements on the **source txn**; no mutex.
- **Close**: set `closing = true`; **wait** until `pins == 0` on a CV (hold **no other locks** while waiting).
- **Mutations**: take **destination txn mutex** only; never nest txn locks.
- **Rule**: do not call user code while holding the dest lock.
- Pin proxy **caches** the txn it pinned to unpin the **same** one.

---

## Transaction Close Protocol
```text
BeginClose():
  closing = true        // reject new pins (TryPin() fails)
  wait until pins == 0  // readers drain
  teardown …
```
`PinProxy` unpins in its destructor; close cannot be starved once `closing` is set.

---

## Same‑Txn Pinning
Default (simple & safe): **always pin the pointer’s owning txn** — even when `source == destination`.  
Optional micro‑opt: behind a build flag, skip pin if `source == destination` for ultra‑hot paths.

---

## Usage Examples
```cpp
PGC::Ref<Foo> r(destTxn);

r.Set(obj);          // encodes direct or promotion under dest lock

r->DoThing();        // pins source during the call, then unpins

{ auto pin = r.Pinned();
  pin->Step1();
  pin->Step2();
} // unpin here

// Optional: eager cleanup of finished promotions
r.Collapse();        // if you provide it
```

---

## Tests (checklist)
- **Same‑txn set**: `Set(obj_same_txn)` → no promotion; `ref->call()` succeeds.
- **Cross‑txn set**: `Set(obj_other_txn)` → promotion exists; after source commit, cleanup via `Clear()`/dtor; optional non‑const `Get()` collapses.
- **Move**: move‑ctor/assign adopt other’s promo; no double‑free; moved‑from is inert.
- **Pin lifetime**: `r->f()` prints `pin → f → unpin`; exception still unpins.
- **Close waits**: thread A loops `r->f()`; thread B `BeginClose()`; B returns after A stops.
- **No deadlocks**: concurrent `Set/Clear/Get()` with only dest‑lock; pins are lock‑free.

---

## FAQs
**Q: Do we ever pin the destination?**  
A: Not for read access via the proxy. Destination locking is for mutation paths only.

**Q: Can promotions leak if we never call non‑const `Get()`?**  
A: No. They are freed via `Set/Clear/move/dtor`, and you can also free at txn‑close processing. An explicit `Collapse()` can provide deterministic cleanup if desired.

**Q: Why does the proxy cache the txn?**  
A: To unpin the **same** txn it pinned in case ownership retargets mid‑call.

---

## Future Knobs
- Build flag to **skip same‑txn pin**.
- **Sharded pin counters** for high contention.
- Background sweeper to free completed promotions not referenced by any base.
