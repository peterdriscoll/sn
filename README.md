# SN (Symbolic Notation)

SN is an experimental logic programming system built around **symbolic expressions**, **conditional value sets (CVSs)**, and **reversible computation**. It is designed for symbolic reasoning, constraint solving, and introspective metaprogramming. SN has its own execution model, debugger, and memory management system, including a custom **Promotional Garbage Collection (PGC)** engine.

---

## Why SN?
Traditional programming languages evaluate expressions to single values. SN allows expressions to resolve to multiple **symbolic values** under different **conditions**, enabling advanced symbolic reasoning. It is especially suited for:

- Symbolic parsing (e.g., splitting strings based on grammar rules)
- Symbolic computation (e.g., Church numerals and combinators)
- Meta-level evaluation (reasoning about code itself)
- Deferred/delayed execution

---

## Key Concepts

### Conditional Value Sets (CVSs)
A CVS is a set of values each paired with a **conjunction of conditions**. SN tracks these possibilities through evaluation, maintaining logical structure.

### Meta Expressions
Expressions in SN can describe and manipulate other expressions. `meta(x)` and `unmeta(x)` let you move between levels of interpretation.

### Stringrefs
Strings are represented as **references to substrings**, allowing symbolic decomposition, lookahead, and constraint propagation.

### Promotional Garbage Collection (PGC)
PGC enables transactional memory with promotion logic that ensures memory is safely reused and updated when symbolic paths diverge. It supports both raw pointers ("backstabbing") and transaction-aware `Ref<T>` ("double dipping").

---

## Features
- **Symbolic execution engine** written in C++
- **Debugger UI** using Angular frontend
- Support for delayed evaluation and symbolic recursion (Y combinator)
- Modular validators like `IsInteger`, `ParseInteger`, `IsFloatingPoint`
- Support for `Define`, `PartialAssert`, and `Evaluate()` logic

---

## Current Status
SN is under active development. Core logic works and is tested. The system has a web-based debugger, but needs more documentation and testing. The goal is to make SN a robust engine for symbolic and logical experimentation.

---

## Getting Started
### Build Requirements
- Visual Studio 2022
- CMake (optional)
- Angular CLI (for the frontend)

### To Run:
1. Build SN backend (C++)
2. Start web debugger
3. Load debugger frontend (Angular)

---

## Contributing
We welcome contributors! Areas needing help:
- Writing unit tests
- Improving documentation
- Transitioning to `Ref<T>` model
- Debugger UI fixes

See [TODO.md] or GitHub issues for tasks.

---

## License
TBD. (Likely open-source)

---

## Author
Peter Driscoll, with help from ChatGPT and a pair of very clever dogs.

---

## Acknowledgements
Inspired by the beauty of functional programming, logic, and the joy of symbolic thinking.

> "In a world full of values, SN dares to think in sets."

