# Promotable Garbage Collector (PGC) -- Derived from C++ Constraints

## 1. C++ Constraints → Requirements

-   **Addresses aren't relocatable**
    -   Once constructed, an object's address is part of the program's
        meaning.\
    -   Requirement: "Moving" an object = allocate new + rebind
        references.
-   **Destructors run exactly once**
    -   RAII requires deterministic destruction.\
    -   Requirement: Old object becomes a cheap shell after promotion.
-   **Pointers/references aren't tracked**
    -   No runtime pointer fix-up in C++.\
    -   Requirement: Add an indirection layer (`MemberRef`) that can be
        retargeted.
-   **Concurrency is explicit**
    -   No implicit safepoints.\
    -   Requirement: Promotions must be explicit, protected by a mutex;
        teardown waits on pins.
-   **Cross-DSO/ABI edges**
    -   Layout and vtables are fixed across modules.\
    -   Requirement: Fix-ups rely only on our handles (`MemberRef`) and
        member registration.
-   **Exceptions & strong guarantees**
    -   Requirement: Promotions must be 2-phase (plan + commit).

## 2. Model

-   **Transaction (arena):** Owner of allocations and destruction
    boundary.\
-   **PGC_Base:** Every GC-managed object derives from this. Tracks
    owner transaction + logical ID.\
-   **MemberRef`<T>`{=html}:** Indirection handle used everywhere. Can
    be retargeted during promotion.\
-   **Promotion strategies:**
    1.  **Double-Dipping** (preferred) -- update RefCores only.\
    2.  **Backstabbing** -- patch raw pointers; legacy bridge.\
-   **Pin:** RAII guard preventing transaction teardown.

## 3. Promotion Semantics

### Algorithm

1.  **Plan**: Traverse graph from root refs. Record all old→new
    mappings.\
2.  **Allocate**: Create new shells in destination transaction.\
3.  **Wire**: Update `MemberRef`s (or raw pointers if registered).\
4.  **Commit**: Atomically retarget cores; mark old objects as promoted
    shells.

### Guarantees

-   Strong exception safety (no observable change if error occurs before
    commit).\
-   Logical identity (`LogicalId`) preserved; raw address changes.\
-   Cross-transaction deref prohibited unless pinned.

## 4. Core Classes (sketch)

``` cpp
struct PGC_Transaction {
  void Pin();
  void Unpin();
};

struct PGC_Promotion {
  enum class Strategy { DoubleDipping, Backstabbing };
  template<class T> T* Request(T* old, PGC_Transaction* dst);
};

struct PGC_Base {
  PGC_Transaction* m_Txn = nullptr;
  uint64_t m_LogicalId = 0;
  virtual void PromoteMembers(PGC_Promotion& ctx) {}
  void MarkPromotedCopy();
};

template<class T>
struct MemberRef {
  T* Get() const;
  void Set(T* ptr);
  bool operator==(const MemberRef& rhs) const;
};
```

## 5. Invariants

1.  Every managed object has a non-null owner txn.\
2.  Object graphs only contain `MemberRef` edges (except explicit raw
    bridges).\
3.  After promotion, no live handle points to old.\
4.  Transactions only destruct when pin count = 0.\
5.  Promotions are serialized by a global mutex.

## 6. Diagnostics

-   Cross-txn deref error: show
    `{field, type, owner_txn, current_txn}`.\
-   Unregistered raw pointer error: fail fast with type + offset.\
-   Accounting: track allocations, promotions, shells, memory net/gross.

## 7. Thread-Safety

-   Pin txn while reading if promotions/teardown possible.\
-   Global mutex around promotion commit.\
-   Reads/writes to `MemberRef::Get/Set` are lock-free except asserts.

## 8. Tests

-   **Basic:** `Promote(A)` → all refs updated.\
-   **Graph:** A→B→C promotes fully.\
-   **Backstab bridge:** Raw pointers patched.\
-   **Exception safety:** Abort leaves world untouched.\
-   **Accounting:** Memory counters match.\
-   **Identity:** LogicalId stable, raw pointers differ.\
-   **Concurrency:** Pin + promote safe.\
-   **Mixed transactions:** Assert with detailed diagnostics.

## 9. Quick Rules

-   Use `MemberRef` everywhere inside GC graphs.\
-   Promotions are explicit 2-phase operations.\
-   Pin when reading from potentially destroyed/promoted txn.\
-   Prefer Double-Dipping; Backstabbing only as migration aid.
## Summary
If every access goes through a retargetable slot (your MemberRef/RefCore), you can “move” by allocating a new object and flipping the slot. All users come back to the slot, so they see the new address automatically. No pin needed for those users.

If anyone holds a raw pointer / out-of-band reference that you can’t immediately retarget, they must Pin the source transaction while using it. The Pin forbids teardown and promotion that would invalidate their raw address.

Quick rule of thumb:

How they access	Safe during promotion?	Needs Pin?	What Pin blocks
Via MemberRef slot	✅ yes (slot retargets)	No	n/a
Via raw pointer / T*	❌ no (address would change)	Yes	Promotion + teardown of source txn

So: promotion proceeds freely for slot users; pins are required only to protect raw-pointer readers.