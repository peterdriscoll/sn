# Blot Pattern — Facade/Data with RefA

*A pin-light, move-friendly object model for C++*

---

## Intent
Provide a clean public class surface while storing data out-of-line, using stable **slot-based handles** (RefA) and **short-lived pins** only when truly needed. Method calls on façades never hold a parent object while invoking into children (“hop then call”).

## Core Ideas (TL;DR)
- **Facade/Data split**: The public class `Foo` is a façade that owns **no fields**; storage lives in `Foo::Data`.
- **Handles, not pointers**: `RefA<T>` is a slot handle. It can be **moved** (rebinding its slot) but **not shared** between different handles’ cores.
- **RefCore rule**: One core per handle; cores are **owned-by-handle** and never shared. Copies create a **new core** bound to the same slot (per current policy); moves transfer ownership.
- **Arrow split**:
  - `RefA<Facade>::operator->()` → returns **Facade\*** (no pin).
  - `RefA<Data>::operator->()` → returns **Pin<Data>** / **ConstPin<Data>** (RAII short pin).
- **No mirrors**: The façade stores only `RefA<Data> m_self`. Reads/writes reach data through `m_self->field`.
- **Hop then call**: Never chain through the parent. Do:
  ```cpp
  auto next = m_self->m_Next;      // copies the handle; parent released here
  if (next) next->DoWork();        // pin applies only to child during the call
  ```
- **Truthiness**: `explicit operator bool()` on `RefA` checks binding **without borrowing**.
- **Const correctness**: `PinProxy` must be able to fetch the transaction for `const T*` as well as `T*`.
- **Pins are optional**: Keep **short-lived pins** for atomic edits when needed; most façade code is pin-free.

## Public Shape
### Façade skeleton (no mirrors)
```cpp
namespace NewStuff {

class Something {
public:
    struct Data {                      // storage only
        std::string           m_Name;
        std::string           m_Description;
        PGC::RefA<Something> m_Next;   // links are handles to façades
    };

private:
    PGC::RefA<Data> m_self;            // the only façade state

public:
    // Attach from data (factory path)
    explicit Something(PGC::RefA<Data> self) : m_self(std::move(self)) {}

    // Attach from another façade handle (same-class private access OK)
    explicit Something(const PGC::RefA<Something>& src)
        : m_self(src->m_self) {}

    void MakeJSON(std::ostream& os) const {
        auto name = m_self->m_Name;            // short pin ends at ';'
        auto desc = m_self->m_Description;

        os << "{ \"name\":\"" << name
           << "\", \"description\":\"" << desc << "\"";

        if (auto next = m_self->m_Next) {
            os << ", \"next\": ";
            next->MakeJSON(os);                // pins only child during call
        }
        os << " }";
    }
};

} // namespace NewStuff
```

### RefA arrow semantics (conceptual)
```cpp
// Facade path (no pin)
template<class U=T> requires IsFacade_v<U>
U* operator->();

// Data path (RAII pin)
template<class U=T> requires (!IsFacade_v<U>)
Pin<T> operator->();
```

### PinProxy const fix (conceptual)
`PinProxy` must be able to call `GetTransaction()` on both `T*` and `const T*`. Either:
- provide `PGC_Base::GetTransaction() const`, or
- in `PinProxy`, cast the **object** to non-const before calling the non-const getter.

## Access Patterns
- **Single hop (read/write)**: `auto v = m_self->field;` / `m_self->field = v;`
- **Cross-object call**: `auto h = m_self->child; if (h) h->Do();`
- **No address-taking across statements**: Do not store `Data*`, `&field`, iterators, or `c_str()` beyond the statement.
- **Atomic multi-field edit**: use a private helper to scope a tiny pin:
  ```cpp
  template<class F>
  void WithData(F&& f) const {
      auto pin = m_self.operator->();  // Pin<Data>
      f(&*pin);
  }
  ```

## Factories (rough)
```cpp
inline PGC::RefA<NewStuff::Something> CreateSomething(PGC_Transaction* txn) {
    auto* d = new NewStuff::Something::Data{};
    PGC::RefA<NewStuff::Something::Data> hData{ d, txn };
    auto* f = new NewStuff::Something(hData);
    return PGC::RefA<NewStuff::Something>{ f, txn };
}
```

## Traits (compile-time dispatch)
- **IsFacade**: `true` if `T` declares nested `Data` (cv-agnostic detection).
- Use it to select façade vs data arrow overloads.

## Do / Don’t
**Do**
- Keep façade APIs returning values/handles; no `T&` to data.
- Use `if (handle)` truthiness (non-borrowing) before cross-calls.
- Keep RefCore **owned-by-handle**; copy → new core bound to same slot; move → transfer.

**Don’t**
- Don’t chain `m_self->child->Call()`; split into two statements.
- Don’t expose `Data` types publicly.
- Don’t keep stable addresses into `Data` across statements.

## Migration Plan (incremental)
1. **Prep primitives**: arrow split, PinProxy const fix, `operator bool()`.
2. **Split class**: move fields to `Foo::Data`; façade holds `RefA<Data>`.
3. **Attach ctors**: from data and from façade handle.
4. **Rewrite methods**: single hop + hop-then-call.
5. **Introduce WithData** only where atomic edits are needed.
6. **Switch call sites** to `RefA<Facade>` and `->Method()`.

## Test Checklist (smoke tests)
- Parent not pinned when calling child (`hop-then-call`).
- Moving parent during child call doesn’t crash.
- Copying handles yields independent promotion state.
- `if (handle)` works and doesn’t borrow.
- `PinProxy` works for `const` façade methods.

## FAQ
- **Why no mirrors?** Simpler invariants; every access goes through the handle. Mirrors can be added later if profiling shows need.
- **Why short pins at all?** Only for atomic multi-field edits or helpers that need a stable `Data*` briefly.
- **Can we drop pins entirely?** Yes if the mover guarantees safety (e.g., RCU/epochs/forwarding), and the codebase forbids address/iterator leakage.

## Naming Ideas
- *Blot Pattern* (current nickname)
- *Facade/Data + Slot Handle*
- *Pin-Light Facade*

---

**Status**: working sketch. Future refinements: nicer factories, accessors (`Name()`, `Next()`), and linting to enforce hop-then-call.

