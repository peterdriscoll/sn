---

# PGC Transactions

A **PGC\_Transaction** in SN (Symbolic Narrowing) provides a structured memory arena in which symbolic operations can occur safely. Each transaction defines a lifetime boundary for allocated memory and handles its own cleanup. This enables memory-efficient symbolic computation, safe rollback, and deferred promotion of surviving data.

---

## Core Concepts

### 1. Memory Blocks

Each transaction allocates memory in fixed-size **blocks**, typically optimized to reduce fragmentation and simplify allocation:

- Blocks are appended as needed.
- Each block tracks how much space is used.
- Objects are allocated sequentially within these blocks.

### 2. Stack and Nested Transactions

Transactions can be **nested**. For example:

```cpp
PGC_Transaction outer;
{
  PGC_Transaction inner;
  // inner lives and dies inside outer
}
```

- Inner transactions allocate independently.
- On destruction, all memory in a transaction is released (except promoted instances).
- This forms a stack-like structure that matches natural evaluation or assertion scopes.

### 3. Destruction Phase

Memory is only reclaimed when the transaction is explicitly **destroyed** (typically via scope exit). At that point:

- All unpromoted memory is deleted.
- Destructors are called for each allocated object.
- Promotions are finalized before destruction to avoid dangling references.

### 4. Promotion Points

A transaction can **promote** instances to a longer-lived transaction (typically its parent). This occurs:

- At the destruction phase, if a promotion request exists.
- Promotion involves copying the memory and updating references.

### 5. Transaction Identification

Each allocated instance can query which transaction owns it:

```cpp
PGC_Transaction* t = instance->GetTransaction();
```

This is used for validation, promotion logic, and memory tracking.

---

## Memory Lifecycle

| Stage       | Action                                   |
| ----------- | ---------------------------------------- |
| Allocation  | Object placed in current block           |
| Use         | Object lives inside current transaction  |
| Promotion   | Object copied to another transaction     |
| Destruction | Memory freed unless promoted             |
| Reuse       | Promotions reused via internal free list |

---

## Debugging Tips

- Use `TotalNetMemoryUsed()` to check net active memory.
- `TotalGrossMemoryUsed()` includes memory in free lists.
- `PromotionUsedMemory()` checks how many promotions are still active.
- Always confirm these are zero after a test to ensure cleanup.

---

This document forms the foundation of the PGC system's memory and lifecycle management. For details on promotion logic, see `promotion.md`.

