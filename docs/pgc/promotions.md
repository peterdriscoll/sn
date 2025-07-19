---

# Promotions

In SN's Promotional Garbage Collection (PGC) system, **promotion** is the process by which an object allocated in one transaction is transferred to another transaction with a longer lifetime. This is necessary when symbolic data must outlive the transaction that created it.

---

## Purpose

Symbolic computations often require **returning values**, **retaining state**, or **building recursive structures** across transaction boundaries. Without promotion, all such objects would be destroyed at the end of their original transaction.

---

## Core Mechanism

### 1. Promotion Requests

When a pointer is updated to reference an object from a different transaction, a **promotion request** is registered.

```cpp
Ref<T> r;
r = objFromShortLivedTransaction;
```

- The `Ref<T>` wrapper detects the cross-transaction assignment.
- It registers a `PGC_Promotion` instance containing:
  - A pointer to the object.
  - A reference to the destination transaction.
  - A back-pointer to the reference (`T**`) so it can be updated after promotion.

### 2. Processing Promotion Requests

At the end of the transaction (destruction phase), all promotion requests are processed:

- If the **source transaction is dying**, and the **destination is still alive**, the object is **promoted** (copied into the destination).
- The pointer in the original reference is updated.
- If the **destination is dying**, or any conditions are invalid, the request is silently discarded.

### 3. Promotion Object Lifecycle

Promotion requests are stored in a linked list and reused:

- Active promotions are tracked using `m_PromoteList`.
- When a promotion is complete or rejected, its object is returned to a **free list** for reuse.

---

## Memory Safety

The promotion system ensures that:

- References do not dangle across transactions.
- Only valid cross-transaction references are promoted.
- All objects are copied exactly once, even in cyclic structures.

To avoid accidental use-after-free, objects being promoted are handled *before* memory is released, and references are updated atomically.

---

## Debugging Notes

- Use `Promotion::PromotionUsedMemory()` to track active promotions.
- `PGC_Promotion::PromoteRequests()` handles the processing loop.
- Conditional debugging hooks can be added during promotion to verify invariants.

---

## Promotion Patterns

| Pattern                         | Behavior                                   |
| ------------------------------- | ------------------------------------------ |
| Cross-transaction pointer write | Registers a promotion request              |
| Source transaction ends         | Request processed, object copied if needed |
| Destination transaction ends    | Promotion rejected, pointer left unchanged |
| Object already promoted         | Promotion reuses existing copy             |

---

## Summary

Promotion is central to SN’s memory safety across symbolic computations. It enables rich, recursive, and persistent structures to be built transactionally while keeping garbage collection deterministic and efficient.

For details on transactions themselves, see `transactions.md`.

