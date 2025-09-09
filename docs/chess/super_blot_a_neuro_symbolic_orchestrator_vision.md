# Super‑Blot: A Neuro‑Symbolic Orchestrator (Vision)

*A compact manifesto for mixed‑mode intelligence: symbolic rules do the heavy lifting fast; neural nets wake only when they truly add value.*

---

## TL;DR
Super‑Blot is a **neuro‑symbolic controller**. It uses fast, explainable **symbolic rules** to handle most decisions and route only the ambiguous or novel cases to **neural specialists**. This delivers **lower latency/cost**, **greater interpretability**, and **robustness** via a safety verifier and clear telemetry. Chess (AOS) is the testbed; the pattern generalizes to Blot’s broader tasks.

---

## Why Now
- **Latency & cost pressure:** Running big NNs everywhere is expensive and slow.
- **Explainability requirements:** Humans want to know *why* a choice was made.
- **Stability:** Symbolic cores degrade gracefully; rules can be unit‑tested.
- **Leverage strengths:** Rules excel at structure and constraints; NNs excel at fuzzy judgments.

---

## Core Architecture

1) **Feature Extractors (cheap)**  
   Deterministic signals from raw inputs: counts, patterns, relations (graphs/rays), simple stats. Must be O(input) and cache‑friendly.

2) **Rule Library (symbolic)**  
   Short, human‑readable rules that produce **proposals** with **score, confidence, rationale**. Rules may be hand‑seeded or learned (simplicity‑penalized). Types:
   - *Detect*: identify advantages/opportunities/errors.
   - *Constrain*: prune unsafe/invalid actions.
   - *Propose*: construct candidate actions/plans (macro‑moves).

3) **Gate/Router**  
   If rules **agree** and confidence ≥ τ → act immediately. Else **escalate** to NN specialist(s). Always apply a minimal safety set.

4) **Verifier (safety net)**  
   Tiny deterministic checks (domain‑specific) to prevent rule‑induced blunders; depth‑limited, allocation‑free.

5) **Neural Specialists**  
   Small models invoked selectively to resolve ambiguity, rank candidates, detect novelty, or provide residual scoring where rules are weak.

---

## Routing Loop (pseudocode)
```pseudo
features = extract(x)
rules = fire_rules(features)  // each: {proposal, score, confidence, why}
S = consensus(rules)
if S.ok and S.conf >= TAU:
    y = decide(S)
    return y, trace(rules)
else:
    y_nn = NN(x, candidates=rules.proposals)
    y = merge(rules, y_nn)   // NN tie-breaker / veto / residual
    ensure verifier(y)
    return y, trace(rules, y_nn)
```

**Consensus** may be majority, weighted agreement, or “no contradictions above severity k”.

---

## Rule Lifecycle
1) **Propose → Test → Promote → Retire** with unit tests & traces.
2) **Learn** new rules from disagreement cases (rule vs NN vs truth) using program synthesis / ILP / sparse decision lists with an **MDL (simplicity) penalty**.
3) **Distill** frequent NN judgments into new rules; add **veto rules** for common failure modes.

---

## Telemetry & Explainability
- **Trace every decision:** which rules fired, scores, confidence, conflicts, final route (rule or NN).  
- **Metrics:** skip rate (NN avoided), latency/cost saved, escalation quality, coverage/precision of rules, drift/OOD triggers.

---

## Safety & Guardrails
- Always include a **minimal safety set** of checks that cannot be skipped.
- **Verifier** must be side‑effect free, deterministic, and cheap.
- **Drift sensors:** feature distribution canaries that force NN escalation when out‑of‑distribution.
- **Caps:** prevent double‑counting when multiple rules cover the same evidence.

---

## Example Domain: Chess (AOS)
Symbolic motifs (pins, forks, skewers, overload, lures, trapped pieces, passed/outside passer, rook‑behind‑passer, king‑safety spikes, fortress, simple zugzwang).  
- **Δadv filter:** Keep moves that increase advantage; always include checks/captures as safety.
- **Two‑sided scoring:** immediate gain minus best reply (swing), with SEE‑lite and escape‑in‑1/desperado gates.
- **Macro‑moves:** treat multi‑move motifs as single branches; use verifier to guard.  
Outcome: shallow, explainable trees that still encode long plans.

---

## Generalization Patterns
- **Eligibility Filters:** fast symbolic predicates prune 80% of candidates before NN.
- **Constraint Satisfaction:** rules define feasible regions; NN scores within the region.
- **Routing:** rules choose a specialist model/few‑shot prompt;
- **Residuals:** NN adds subtlety only where rules report uncertainty.

---

## Minimal Deliverable (Week‑1 Pilot)
- 5–8 rules in one slice (e.g., chess motifs or Blot’s eligibility checks).
- Gate τ, consensus strategy, and a tiny verifier.
- Telemetry with decision traces.
- A/B vs NN‑only: target ≥70% decisions handled by rules at equal/better quality, with ≥50% latency saving.

---

## Milestones
- **M0:** Core loop + telemetry + 3 rules; verifier on; unit tests.
- **M1:** 8–12 rules; learned weights; NN tie‑breaker integrated.
- **M2:** Drift sensors; rule synthesis from disagreements; promotion/retirement tooling.
- **M3:** Cross‑domain reuse; shared rule patterns; dashboards.

---

## Risks & Mitigations
- **Rule brittleness:** verifier + NN veto; continuous evaluation on canaries.
- **Overfitting:** MDL penalty; A/B across datasets & versions.
- **Complexity creep:** strict rule budget; sunset rarely‑firing rules.
- **Silent regressions:** mandatory decision traces and CI tests per rule.

---

## Open Questions
- Best consensus function for mixed confidence signals?
- How to calibrate rule confidence vs NN scores robustly?
- What’s the minimal safety set per domain that catches >95% failures?
- When do macros (2–3 move plans) pay off vs normal branching?

---

## Appendix A — Rule Template (symbolic)
```text
Rule: <name>
Inputs: <features used>
Pre‑conditions: <sanity checks>
Logic: <short decision list / predicates>
Output: {proposal, score (Δ), confidence, rationale}
Tests: <unit cases where it must fire / must not fire>
```

## Appendix B — Gating Config (example)
```text
TAU = 0.8
Consensus = (no conflicts above severity=High) AND (weighted_agreement >= 0.7)
SafetySet = {mandatory checks / domain invariants}
Escalate if OOD(feature_stats) OR low confidence OR conflicting proposals
```

## Appendix C — Chess Motif Signals (starter)
- Pin/Skewer: absolute/relative, attacker/defender value ordering, escape‑in‑1, desperado, stackability.
- Fork: targets (≥1 high‑value), safe interpose?, ΔSEE ≥ 0.
- Overload: obligations per defender; cheapest break cost.
- Lure/Deflection: forced relocation to dominated square (SEE < 0) or broken defense.
- Trapped: mobility mask empty or all exits losing (SEE < 0).
- Passed/Outside passer + rook‑behind; king‑safety holes; fortress/locked; only‑moves metric.

---

**Bottom line:** Super‑Blot routes routine decisions through fast, explainable rules and reserves neural compute for what it does best. It’s faster, cheaper, safer — and more fun to debug.

