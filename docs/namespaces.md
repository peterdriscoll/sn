# Namespace Guidelines for SN and PGC

This document explains the intended purpose and best practices for using namespaces in the SN project.

## Overview

The SN project uses multiple namespaces to separate concerns and clarify the intended use of components.

| Namespace | Intended Audience | Description |
|----------|-------------------|-------------|
| `PGC`     | Internal Core     | Core logic of the Promotional Garbage Collector system. Avoid using this directly outside implementation. |
| `PGCX`    | SN Internal       | Convenience aliases and tools for internal SN use. May be unstable or subject to change. |
| `skynet`  | End Users         | Public-facing SN logic API. Safe and intended for general use. |
| `std`     | Standard Library  | As expected. Should be used carefully. |

---

## Guidelines

### ✅ Avoid wildcard `using namespace` in headers

Using `using namespace std;`, `using namespace PGCX;`, or `using namespace skynet;` in headers can create name conflicts and pollute the global namespace.

**Do not do this:**
```cpp
// BAD in headers!
using namespace std;
using namespace PGCX;
```

### ✅ Prefer explicit qualification

Use fully qualified names in most source files, especially for internal systems:

```cpp
PGCX::MemberRef<MyType> ref;
PGC::PGC_Transaction* txn = GetTransaction();
```

### ✅ In tests and example code

It's acceptable to use:

```cpp
using namespace skynet;
```

To improve readability in end-user-facing tests or demos. `skynet` is the API layer.

---

## Suggested Local Aliases (if needed)

You can use local `using` aliases inside functions or classes for convenience:

```cpp
void DoSomething() {
    using Ref = PGCX::MemberRef<MyType>;
    Ref x;
}
```

---

## Summary

- ❌ Avoid `using namespace` in headers.
- ✅ Use `skynet` for public SN-facing code.
- ✅ Use explicit names for `PGC` and `PGCX` in implementation.
- ✅ Limit `using namespace` to .cpp files and test/demo contexts.