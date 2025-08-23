Memory Management in C++
1. Manual new / delete

Mechanism: Programmer allocates and frees explicitly.

Pros:

No runtime overhead beyond malloc/free.

Full control of lifetime.

Cons:

High risk of leaks, double frees, dangling pointers.

No structure for ownership — must be tracked by convention.

Contrast with PGC:

PGC eliminates manual frees; transactions and promotion ensure objects stay alive until safe.

PGC provides identity continuity across moves (something manual delete/alloc doesn’t).

2. RAII + Smart Pointers

Mechanism: Ownership modeled with std::unique_ptr, std::shared_ptr. Destructor frees automatically.

Pros:

Simple, idiomatic C++.

unique_ptr: zero overhead, single owner.

shared_ptr: reference counting, safe sharing.

Cons:

shared_ptr overhead (atomic refcount).

Cycles leak without weak_ptr.

Contrast with PGC:

Smart pointers don’t allow promotion across ownership domains — moving an object from one arena to another requires deep copies.

PGC supports transactional promotion — all pointers update together.

PGC is more like shared_ptr in spirit, but coordinated across an entire object graph.

3. Region / Arena Allocation

Mechanism: Objects allocated in a region, all freed at once when region ends. (e.g. llvm::BumpPtrAllocator, std::pmr::monotonic_buffer_resource).

Pros:

Very fast allocation/deallocation.

Simplified lifetime (bulk free).

Cons:

No object-by-object reclamation.

Leaks if region is too long-lived.

Contrast with PGC:

A Transaction in PGC is an arena — but with the extra twist that objects can be promoted to a longer-lived arena instead of dying with the current one.

Promotion lets you “rescue” values without breaking the region model.

4. Reference Counting (intrusive / external)

Mechanism: Each object tracks number of owners; deletes when count hits zero.

Pros:

Predictable destruction (no pauses).

Works without whole-program analysis.

Cons:

Overhead per object, per assignment.

Cyclic references leak.

Contrast with PGC:

PGC does not count references. It uses graph traversal at promotion time.

No refcount overhead on every assignment.

Handles cycles naturally since promotion rewires entire graphs, not per-edge decrements.

5. Tracing Garbage Collectors (rare in C++)

Mechanism: Periodically walk object graph and reclaim unreachable objects. (e.g. Boehm GC).

Pros:

Automatic cycle handling.

Familiar to Java/C# programmers.

Cons:

Stop-the-world or incremental overhead.

Unidiomatic in C++ (RAII fights with nondeterministic destruction).

Contrast with PGC:

PGC is deterministic — promotion/teardown happens at transaction boundaries, under explicit control.

No background collector, no surprise pauses.

Explicit pinning makes cross-transaction safety visible, not hidden.

6. Move Semantics

Mechanism: Ownership transferred via std::move; destructor disabled in moved-from object.

Pros:

Zero overhead when used correctly.

Explicit at call sites.

Cons:

Tedious when objects are deeply shared.

Not suitable for bulk promotion of graphs.

Contrast with PGC:

PGC’s promotion is like a transactional, graph-aware move: the whole reachable object set is “moved” at once, with slots rewired.

Preserves logical identity while moving storage.

Summary Table
Scheme	Ownership Model	Cycles Safe?	Deterministic?	Graph-Aware Moves?	Overhead
Manual new/delete	Programmer	N/A	Yes	No	Low, risky
RAII + unique_ptr	Single owner	Yes	Yes	No	Very low
RAII + shared_ptr	Shared (refcount)	No	Yes	No	Medium (atomics)
Arena / Region	Region lifetime	N/A	Yes	No	Very low
Refcounting (intr.)	Shared (refcount)	No	Yes	No	Medium
Tracing GC	Reachability graph	Yes	No	Implicit	Medium–high
PGC	Transaction + slots	Yes	Yes (explicit)	Yes (promotion)	Moderate (promotion walks)
Big Picture

PGC = RAII + Arena + Graph Promotion.
It fits naturally into C++’s deterministic destruction model, but provides a migration path for objects across regions — something none of the other schemes offer.

It trades background automation (like tracing GC) for explicit, predictable promotions and the safety of RAII invariants.