## Design Philosophy: Reasoning Without Branching

This document outlines the core motivations and philosophical principles behind the design of SN.

In traditional logic systems and symbolic computation, reasoning often leads to branching — a combinatorial explosion of paths when working backward from a goal or constraint. This branching is typically handled by search, pruning, or heuristic methods.

**SN takes a different approach.**

### The Insight

> *Forward reasoning is deterministic. Backward reasoning seems ambiguous — unless you treat all the possibilities as a single value.*

SN is built on the principle that **multiple possible values can be represented together as one Conditional Value Set (CVS)**. This means a function like `f(x) = y` is always invertible: even if `f⁻¹(y)` has many solutions, they are carried symbolically as a unified set, not explored one by one.

This eliminates the need for search or case analysis. Every function becomes a logical constraint. Branching disappears — replaced by **symbolic narrowing** within a relational framework.

### Why It Matters

* SN avoids the exponential complexity of classical symbolic engines.
* It enables invertibility and bi-directional reasoning by design.
* It treats ambiguity as a first-class citizen, not as an exceptional case.

In SN, **reasoning is just constraint propagation over CVSs**. Even when values are unknown or ambiguous, they are narrowed by interaction with other constraints — without splitting the world into separate branches.

# Function Application in Conditional Value Sets (CVSs)

In SN, both functions and values can be represented as **Conditional Value Sets** (CVSs). Each CVS is a collection of values, each valid under a mutually exclusive and exhaustive condition. This allows for reasoning with ambiguity, symbolic inversion, and non-deterministic computation.

## CVS Structure

A CVS is written as:

```
{ (value₁ | condition₁), (value₂ | condition₂), ... }
```

This means:

* `value₁` is valid when `condition₁` is true,
* `value₂` is valid when `condition₂` is true,
* And so on, with the conditions being mutually exclusive and exhaustive.

## Applying a Function CVS to a Value CVS

Let:

```
f = { (f₁ | C₁), (f₂ | C₂), ..., (fᵢ | Cᵢ) }
x = { (x₁ | D₁), (x₂ | D₂), ..., (xⱼ | Dⱼ) }
```

Then the function application `f(x)` is defined as:

```
f(x) = { (fᵢ(xⱼ) | Cᵢ ∧ Dⱼ) for all i, j }
```

## Explanation

Each function `fᵢ` is applied to each value `xⱼ`, producing a result `fᵢ(xⱼ)` that is valid only when both `Cᵢ` and `Dⱼ` are true. The result is itself a CVS, with conditions formed by the conjunctions `Cᵢ ∧ Dⱼ`.

This removes the need for branching or procedural logic: the combination of possibilities is encoded explicitly in the structure.

## Benefits

* Symbolic reasoning is preserved.
* Evaluation can be deferred or inverted.
* Compatible with narrowing and unification.
* Efficient representation of multiple possibilities without branching trees.

## Example

Let:

```
f = { (add1 | A), (square | B) }
x = { (2 | C), (3 | D) }
```

Then:

```
f(x) = {
  (add1(2) = 3 | A ∧ C),
  (add1(3) = 4 | A ∧ D),
  (square(2) = 4 | B ∧ C),
  (square(3) = 9 | B ∧ D)
}
```

The resulting CVS captures all combinations of functions and arguments, paired with precise conditions under which each applies.

---

This formulation is a cornerstone of SN, enabling symbolic function application and clean inversion logic across all types.

