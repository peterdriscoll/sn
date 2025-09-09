# Neuro‑Symbolic Chess Engine (AOS)
*A compact, explainable engine for SN tests that searches only advantage‑creating lines*

---

## 1) One‑paragraph summary
We build a tiny **neuro‑symbolic chess engine** where **symbolic rules** (pins, forks, overloads, lures, trapped pieces, passed pawns, king‑safety spikes, fortress detection, simple zugzwang pressure) **control the search**, and a **small verifier** (3–5 ply tactics or a tiny learned tie‑breaker) prevents blunders. The core loop is **Advantage‑Only Search (AOS)**: keep and prioritize moves that *create or lock in advantage* (Δadv > 0), plus a minimal safety set (checks/captures). This goal‑directed tree is shallow but encodes multi‑move plans via motifs (“macro‑moves”), giving practical strength vs large NN evals in the right structures while staying fully explainable for SN.

---

## 2) Motivation
- Shrink the search tree to **goal‑directed** branches.
- Provide an **explainable** testbed for SN (which rules fired, why a move was chosen).
- Exploit known **engine blind spots**: fortresses, zugzwangs, locked structures, value‑ordering traps around pins/overloads.

---

## 3) Core principle: Advantage‑Only Search (AOS)
- Compute fast features; retain moves where **Δadv > 0** according to symbolic motifs.
- Always include a **safety set** (all checks + non‑losing captures) to avoid missing tactics.
- **Order** by Δadv, then SEE (static exchange evaluation) and forcing strength.
- **Extend** when a *new decisive motif appears*; **prune** quiets that don’t raise advantage and don’t prevent an opponent spike.

**Pseudo‑AOS**
```text
function AOS(node):
  feats = detect_motifs(node)
  if decisive(feats): return WIN/DRAW as applicable
  C = { m in legal(node) | Δadv(node,m) > 0 } ∪ {checks,captures}
  order_by(C, Δadv, SEE)
  for m in C:
    if not tactical_verify(node,m, depth=3..5): continue
    child = play(node,m)
    if AOS(child) == WIN: return WIN
  return UNKNOWN  // fall back: small quiescence or draw plan
```

---

## 4) Motif catalog (symbolic, fast)
Each detector returns a small tuple (not just bool). Scores reference **value ordering** (attackers/defenders sorted by piece value), **escape‑in‑1**, **desperado**, and **stackability**.

- **Pins** (absolute/relative)
  - Detect slider → blocker → target lines; classify absolute vs relative.
  - Score ↑ if `min(attacker_value) < value(pinned)` and **no escape in 1** (hop with check/counter, safe interpose).
  - **SEE‑lite along line**; discount by **desperado_gain** of the pinned piece.
- **Skewers**
  - Like pins but target behind is higher value; similar SEE, escape rules.
- **Forks** (knight/pawn/queen)
  - Double‑attack of ≥ one high‑value and one loose/undefended target; veto if safe interpose exists.
- **Overload**
  - A defender is obligated to cover 2+ critical duties; compute cheapest break cost; bonus if our forcing move creates the overload now.
- **Lure / Deflection**
  - Forcing move that relocates a piece to a dominated square (SEE < 0) or breaks a key defense.
- **Trapped piece**
  - Mobility mask empty or all exits lose by SEE.
- **Passed / Outside passed pawn + Rook‑behind‑passer**
  - Race geometry (king distance, square‑of‑pawn); structure bonus; rook‑behind bonus.
- **King‑safety spike**
  - Open files/diagonals into king + available checks next ply; penalize if opponent has forcing checks that refute.
- **Fortress / locked structure detector (hard clamp)**
  - Opposite bishops with no pawn breaks, sealed chains, wrong‑colored bishop fortresses → clamp eval to 0 and guide to repetition/50‑move.
- **Zugzwang pressure (simple)**
  - “Only‑moves” count increases for them; triangulation available.

---

## 5) Pin/Skewer scoring (worked example)
**Inputs:** pinned piece value `Vp`, attacker sequence `A↑` (cheap→expensive), defender sequence `D↑`, `escape_in_1 ∈ {none, hop_check, hop_counter, interpose}`, `desperado_gain`.

**Score stub:**
```
if relative_pin and min(A↑) ≥ Vp:   score = 0
else:
  net = SEE_line(pinned_square, A↑, D↑, victim=Vp)
  if escape_in_1 != none: net -= big
  net -= desperado_gain
  score = max(0, net) * weight
```

---

## 6) Δadv vector & SEE‑lite
Represent advantage as a small vector (lexicographic compare):
1) mate/decisive tactic, 2) material SEE, 3) winning passer potential, 4) king‑safety spike, 5) immobilization (trapped/overload).
Use a **SEE‑lite** specialized per motif (e.g., along a pin line or target square) rather than full SEE everywhere.

---

## 7) Verifier / learned tie‑breaker (optional)
- **Verifier:** a 3–5 ply tactical check (captures, checks, recaptures) to veto rule‑induced blunders.
- **Tie‑breaker:** tiny linear/NN model for fuzzy choices (king texture, long‑term piece quality). It **never drives search**; only breaks ties or vetoes.

---

## 8) Explainability & logs (SN‑friendly)
For each move considered: record which motifs fired, their Δadv contributions, SEE results, escapes/desperado adjustments, and verifier decisions. Produce a compact trace so SN tests can assert behavior.

---

## 9) Minimal SN API surface
`SN::ChessBoard` (readonly queries): `turn()`, `legal_moves()`, `attacks(sq)`, `is_attacked(sq,side)`, `ray(sq,dir)`, `piece_on(sq)`, `king(side)`, `see(from,to)`, `distance(a,b)`, `pawn_structure()`.

Motif extractors (pure): `detect_pins`, `detect_forks`, `overload_map`, `trapped_pieces`, `passed_pawns`, `rook_behind_passer`, `king_holes`, `fortress_locked`, `only_moves(side)`.

---

## 10) Test plan
- **Perft** to depth N (move‑gen correctness).
- **Unit FENs** per motif (expected Δadv and escape/SEE gates).
- **A/B harness** vs Stockfish/Leela at low time: solve rate, time to first winning line, draw‑save rate in fortresses.
- **Regression**: log which rules fired; diff across versions.

---

## 11) Milestones (tiny steps)
- **V0**: Board core + perft; motifs: Pins, Forks, Rook‑behind‑passer; AOS move‑ordering; 3‑ply verifier.
- **V1**: Overload, Lure/Deflection, Trapped; king‑safety spike metrics.
- **V2**: Fortress/locked detector (hard clamp), simple zugzwang pressure; optional tie‑breaker.

---

## 12) Risks & mitigations
- **Brittleness** → verifier gate; safety set (checks/captures) always included.
- **Rule explosion** → MDL/sparsity; drop rules that don’t move Elo.
- **Double‑counting** → cap combined bonuses per piece/square.
- **Overfitting to engines** → test across engine versions/configs.

---

## 13) Open questions
- Best minimal feature set for king‑safety that’s cheap yet predictive?
- How deep should verifier be to catch >95% of rule‑induced blunders?
- Macro‑move scripting: when to treat a 2–3 move plan as a single branch?

---

## 14) Next actions (when brain online)
- [ ] Define Δadv tuples for **Pin**, **Fork**, **Overload** (values & gates).
- [ ] Specify SEE‑lite procedures for pin line and fork targets.
- [ ] Draft `SN::ChessBoard` header (queries only) + perft seeds.
- [ ] Implement `detect_pins`, `detect_forks` with logs; wire AOS ordering.
- [ ] Create motif FEN suite and expected traces.

