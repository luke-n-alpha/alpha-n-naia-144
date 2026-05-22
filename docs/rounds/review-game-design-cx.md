# Game Design Review (CX) — 2026-05-22

**Reviewer:** Codex (CX)
**Target:** `docs/game-design.md`
**Reference contracts:** `docs/contracts/tech-stack.md`, `docs/contracts/interfaces.md`, `docs/contracts/tile-spec.md`, `docs/contracts/size-budget.md`, `docs/contracts/game-design.md`
**Verdict:** NEEDS_REVISION

---

## Summary

The current design is broadly implementable in C99 within the 1.44MB disk budget. It covers all declared `SceneType` values and does use Korean jamo tiles as a gameplay mechanic rather than only as decoration.

The reason for `NEEDS_REVISION` is not overall scope. The main problems are contract alignment and mechanic specificity: the document does not yet explain the player-facing jamo manipulation loop precisely enough to de-risk implementation against the current interface/tile contracts.

---

## Review Answers

### 1. Is it implementable in C99 within 1.44MB?

**Yes, likely.**

Reasons:
- Scope is small: 4 chapters, text-heavy structure, no v1 audio, 16-color output, simple save data.
- The main systems fit the current contracts: text exploration, dialog, branching state, and one Pixel-mode minigame.
- The disk budget leaves large headroom: `size-budget.md` reserves 550 KiB fixed and 890 KiB slack.

Implementation risk exists in one area:
- The Pixel-mode rhythm puzzle is feasible in C99, but only if kept as a lightweight timing/pattern scene rendered through the existing pixel buffer API. If it grows into a full action subsystem, the design will drift away from the current contract assumptions.

### 2. Are all `SceneType`s covered?

**Yes.** The design explicitly covers all scene types declared in `interfaces.md`.

| `SceneType` | Covered in design |
|-------------|-------------------|
| `SCENE_TITLE` | `TITLE` |
| `SCENE_CAFE` | `CAFE` |
| `SCENE_ATELIER` | `ATELIER` |
| `SCENE_BATTLE` | `BATTLE` |
| `SCENE_DIALOG` | `DIALOG` |
| `SCENE_ENDING` | `ENDING` |

### 3. Does it use Korean jamo tiles as gameplay mechanic?

**Yes, but the mechanic needs tighter definition.**

Positive:
- The core puzzle is based on jamo brightness patterns.
- `naia` is expressed through brightness traces.
- Progress depends on restoring brightness order, not just reading ordinary text.

Current gap:
- The design says the player "aligns brightness" and "restores memory," but does not define the exact player operation: select/swap/rotate/cycle/match/trace. That makes the central jamo mechanic conceptually valid but operationally underspecified.

### 4. Issues

#### Finding 1 — Core jamo puzzle loop is underspecified

**Severity:** High

The design establishes jamo brightness as the central mechanic, but does not define the atomic player action well enough for implementation. The contract stack already defines glyph size, brightness mapping, and scene APIs, but the design stops at a thematic level.

Missing details:
- What exactly can the player do to a jamo pattern?
- Is the unit of interaction a cell, a tile triplet, a row, or a waveform lane?
- What counts as a solved state?
- How is the puzzle state represented for save/load and branching?

Revision needed:
- Add one concrete ruleset for the Text-mode puzzle and one for the Pixel-mode rhythm puzzle.

#### Finding 2 — Hangul composition model and puzzle presentation are not aligned

**Severity:** High

`interfaces.md` says composed Hangul is internally decomposed and rendered as jamo tiles, with cho/jung/jong placed across adjacent cells. The game design assumes recognizable brightness patterns, but does not explain whether puzzles operate on:
- decomposed jamo tiles,
- complete syllable blocks,
- or abstract brightness-only tokens.

Without that clarification, the gameplay can easily drift into something that ignores the actual jamo tile spec.

Revision needed:
- State explicitly that puzzles operate on decomposed jamo tiles and describe how those tiles appear on the 80×25 grid.

#### Finding 3 — Scene flow is covered, but transition rules are absent

**Severity:** Medium

All `SceneType`s exist, but the transitions between them are not specified. This matters because the interfaces expose `scene_enter`, `scene_exit`, `scene_update`, and `scene_handle_key`, so the design should define when exploration switches to dialog or battle/puzzle scenes.

Revision needed:
- Add a minimal transition map such as `CAFE -> DIALOG`, `ATELIER -> BATTLE`, `BATTLE -> DIALOG/ENDING`.

#### Finding 4 — Scope and text budget are plausible, but content volume guardrails are missing

**Severity:** Medium

The design is likely to fit the 150 KiB scenario budget, but it does not define any limits per chapter, per branch, or per ending. Since this project is contract-driven and size-budget-driven, the design should reflect that discipline.

Revision needed:
- Add simple content caps, for example chapter line count targets and ending text limits.

#### Finding 5 — Document path ambiguity can cause review drift

**Severity:** Low

There are two game-design documents:
- `docs/game-design.md`
- `docs/contracts/game-design.md`

The first is the actual merged design. The second is a creation contract for generating a design. This is manageable, but the naming is easy to confuse during later rounds.

Revision needed:
- Rename or clearly distinguish the contract document in a future cleanup, for example `docs/contracts/game-design-contract.md`.

---

## Conclusion

The design is viable and on the right scale. It satisfies the high-level constraints better than a typical overbuilt RPG pitch would, and it does cover the full scene set while using jamo brightness as real gameplay material.

It should still be revised before approval because the central mechanic is not yet specified at the rule level required for reliable C99 implementation and contract-level verification.
