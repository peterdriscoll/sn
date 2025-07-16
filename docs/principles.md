---

# Core Principles of SN Reasoning

SN (Symbolic Narrowing) is based on a small set of ideas that allow symbolic reasoning to proceed in a mostly-deterministic fashion. This page introduces the foundational principles that make SN work effectively and avoid exponential branching.

---

## 1. Symbolic Narrowing over Conditional Value Sets (CVSs)

The foundation of SN is the ability to treat a value as a Conditional Value Set: a set of possible values, each paired with a mutually exclusive condition.

This allows symbolic reasoning to proceed without choosing a specific value too early. For example:

x.In(Set(3, 4))
f.In(Set(double, triple))
f(x)

This evaluates to the symbolic set of all f_i(x_j) combinations:

f(x) = Set(f_i(x_j)) for f_i in f, x_j in x

This is the full expansion of f(x) across the Cartesian product of possible fs and xs, evaluated symbolically.

The previous construct Collapse({a, b, c}) is being phased out in favor of explicit set membership using Set(...) and .In(...).

---

## 2. Minimum Cardinality Principle

In many cases, symbolic evaluation may yield multiple consistent answers. SN uses a guiding principle to resolve ambiguity:

> **Choose the CVS with the fewest branches (i.e., minimum cardinality).**

This principle avoids unnecessary nondeterminism and helps the system converge more quickly.

For example:

```sn
If(x == 3, 5, 5) => 5
```

rather than:

```sn
Collapse({If(x == 3, 5), If(x != 3, 5)})
```

---

## 3. Delayed Processing

Sometimes an expression cannot be evaluated because it depends on unknowns. Rather than fail, SN defers evaluation until more is known.

This is done by *delaying* the call:

```cpp
Var v;
PartialAssert(v == f(x));
return v;
```

The delayed evaluation is stored and retried when new information becomes available.

This enables SN to:

- Safely assert partial relationships
- Avoid premature failure
- Support infinite and recursive structures
- Allow eventual symbolic resolution

---
4. MIR: Mutual Information Resolution

When combining multiple Conditional Value Sets (CVSs), SN assumes a conjunctive structure for each condition in the result. Each condition is a conjunction of simple conditions, and each simple condition belongs to a well-defined condition set (such as x == 1, x == 2, etc.).

If a simple condition from a condition set does not appear in the final CVS, SN assumes it to be false.

This follows from the logic that, in a condition set with mutually exclusive and exhaustive possibilities, the absence of one condition implies the presence (truth) of another — and thus the missing condition must be false.

This process helps SN eliminate unnecessary branches and tighten constraints, leading to more efficient symbolic evaluation.

## Summary

| Principle           | Description                                                              |
| ------------------- | ------------------------------------------------------------------------ |
| Symbolic Narrowing  | Operate over sets of values with conditions (CVSs)                       |
| Minimum Cardinality | When multiple options are valid, prefer the one with the fewest branches |
| Delayed Processing  | Postpone evaluation until it becomes possible                            |

These principles work together to make symbolic computation tractable, expressive, and logically consistent in SN.

