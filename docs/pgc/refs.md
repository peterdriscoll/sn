---

# Refs

The `Ref` system in SN (Symbolic Narrowing) provides memory-safe, transaction-aware references that support promotion and lifetime management. Three main types of references are used:

---

## 1. `MemberRef<T>`

`MemberRef<T>` is a lightweight reference intended to be used as a **member variable** inside a class derived from `PGC_Base`.

### Characteristics:
- Stores a raw pointer to `T`.
- Assumes the owning class is derived from `PGC_Base`, which allows it to determine the current transaction.
- Does **not** store a transaction itself.
- On assignment, it can request promotion if the object being assigned resides in a dying transaction.
- Accesses to the transaction are routed through the owning object.

### Usage:
Macro-generated getter/setter methods in the containing class ensure that transaction information can be propagated.

---

## 2. `Ref<T>`

`Ref<T>` inherits from `PGC_Base` and is designed to wrap references **on the stack or as members of standalone objects**.

### Characteristics:
- Contains an internal `MemberRef<T>`.
- Manages its own transaction context.
- When assigned a pointer from another transaction, it captures that transaction and registers a promotion request.
- Used to safely bridge transaction boundaries and to hold promoted data temporarily.

### Common Use Cases:
- Temporary objects.
- Parameters passed across nested transactions.
- Holding intermediate results that may be promoted.

---

## 3. `SRef<T>`

`SRef<T>` is a **simple, shallow reference** intended for safe access to objects **within the same transaction**.

### Characteristics:
- Stores only a raw pointer to `T`.
- Does **not** trigger or manage promotion.
- Records the originating transaction's "live" flag and asserts safety on access.
- Used for transient references while a transaction is alive.

### Safety:
- If the transaction is destroyed, any attempt to dereference the `SRef<T>` will assert.
- Useful for efficient local computation without promotion overhead.

---

## Summary Table

| Type         | Promotion Capable | Owns Transaction | Typical Use                     |
|--------------|-------------------|------------------|----------------------------------|
| `MemberRef`  | Yes (via owner)   | No               | Members of `PGC_Base` classes    |
| `Ref`        | Yes               | Yes              | Stack or standalone references   |
| `SRef`       | No                | No               | Safe, local access within scope  |

---

Each type plays a specific role in balancing **safety**, **performance**, and **lifetime control** within SN's PGC system. Choosing the right kind of reference ensures correctness and avoids dangling pointers across symbolic transactions.

For details on how promotions work, see `promotions.md`. For background on transactions, see `transactions.md`.

