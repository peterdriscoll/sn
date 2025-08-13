# Order-Free Deterministic GUID/Time Sources

**Status:** Early Design — concept and vision, not yet implemented.

---

## 1. Motivation

Databases, distributed systems, and symbolic engines like SN often rely on:
- **GUIDs** to uniquely identify entities or events.
- **Timestamps** to record ordering and time of events.

In most systems, these values are generated imperatively from:
- A **global UUID generator** (random or time-based).
- The **system clock**.

While convenient, this approach introduces problems:
- **Order dependence** — reordering operations changes GUID/time values.
- **Non-reproducibility** — rerunning the same operation produces different values.
- **Testing difficulty** — values change between runs, making it hard to compare outputs.
- **Debugging complexity** — impossible to replay an operation exactly as it occurred.

We want **pure, deterministic** GUID/time generation:
- Same initial state ⇒ same sequence of GUIDs/times.
- No hidden global state.
- Order-insensitive when independence is possible.

---

## 2. Existing Approaches and Their Limits

### UUIDv7 / ULID
- Include a timestamp + randomness for sortability.
- Good for DB indexing, but not reproducible from a seed.

### Logical Clocks (Lamport, Vector, Hybrid)
- Deterministic ordering without wall-clock.
- Not intended for unique ID or timestamp minting.

### Deterministic Replay
- Record and replay execution timing/values.
- Useful for debugging, but relies on logs rather than pure generation.

### Content-Addressed IDs (Merkle, Git)
- IDs derived from content hashes — stable for identical content.
- Not applicable when IDs must be unique for *different* but related events.

**Conclusion:** None of these combine **order-free generation**, **seed-based reproducibility**, and **integration with pure symbolic evaluation**.

---

## 3. Core Idea

Treat GUID/time as a **splittable, seeded, order-independent capability**:

- Start with a **root generator**: `(GUIDSeed, TimeSeed)`.
- Derive independent generators via:
  - **Split** — produce two independent generators from one.
  - **Child(name)** — derive a generator deterministically from a name/path.
  - **Label-based** — derive directly from a stable label and global seed.

- Generators are **immutable**: using them produces a value and a *new* generator.

This is analogous to pure functional RNGs, but specialized for GUIDs/timestamps.

---

## 4. Mechanics

### Generator State
```text
struct Gen {
    guidSeed   : 256-bit
    timeSeed   : 256-bit
    counter    : uint64
}
```

### Operations
```sn
(Guid, Gen) GuidNext(Gen g)
(Time, Gen) TimeNext(Gen g)
(Gen, Gen)  Split(Gen g)
Gen         Child(Gen g, String name)
String      MakeGuid(Context ctx, String label)        // pure, order-free
Long        LogicalTime(Context ctx, String label)     // pure, order-free
```

- **GuidNext/TimeNext**: sequential stream (replayable from seed).
- **Split**: independent streams, removing order dependence.
- **Child(name)**: deterministically derived stream from a stable name.
- **MakeGuid/LogicalTime**: label-derived, no generator threading.

---

## 5. Benefits

- **Order independence**: Parallel branches generate IDs/times without affecting each other.
- **Reproducibility**: Same seed + same sequence of calls ⇒ same results.
- **Testability**: Can verify outputs exactly; no flakiness from clock/UUID randomness.
- **Purity**: Closed terms remain constant; no hidden global state.

---

## 6. Applications in SN

- Deterministic evaluation of functions involving GUID/time.
- Symbolic replay of database operations with identical identifiers.
- Safe parallel symbolic evaluation without order-based differences.
- Unit tests for functions that would otherwise depend on real time.

Example:
```sn
(ga, gb) = Split(g0);

(id1, ga1) = GuidNext(ga);
(id2, gb1) = GuidNext(gb);
// id1 and id2 independent; order of calls irrelevant
```

---

## 7. Open Questions

- Should SN expose a **first-class generator object** to user code, or only provide label-based derivation?
- How should generator seeding be handled in distributed contexts?
- Should there be a standard mapping from **AST path → Child generator** for automatic purity?
- How to integrate with imperative operator overloading without leaking hidden state?

---

## 8. Next Steps

- Prototype `Gen`, `GuidNext`, `TimeNext`, `Split`, `Child` in C++.
- Test both thread-and-return and pass-only usage patterns.
- Evaluate label-based derivation as a default for pure code.
- Explore integration into SN’s transaction/context model.
