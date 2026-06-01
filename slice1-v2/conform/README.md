# conform gate — contract↔code drift detection (deterministic, 0 tokens)

Stops an uncommitted agent from looping for hours on a **drifted base** (the
daf35d0 false-success class: a build that passes surface gates while diverging
from its contract). The judge is a pure-Python oracle — no LLM, no tokens —
mirroring `verify-contract-conformance` in naia-adk.

## Pieces
| file | role |
|------|------|
| `oracle.py` | deterministic drift oracle (`drift_set_typelevel`), vendored from naia-adk |
| `check.py` | C symbol extractor + per-region check + loop-state; `--all` / `--file` |
| `manifest.json` | regions = (header, source, expect) — `expect` is the sanity gate |
| `../../.agents/hooks/conform-gate.js` | PostToolUse(Edit\|Write) → runs the check, surfaces STOP |

## How it breaks the loop
After each edit of a C source/header, the hook checks that region:
- **clean** → silent.
- **drift** (1..2 edits) → `systemMessage`: "reconcile contract↔code before adding more".
- **halt** (≥3 edits unresolved) → `block`: "STOP — you are looping on a drifted base".

The signal comes from **outside** the agent's loop, so a drifted agent can't keep
flailing. Reconciling the contract resets the region's counter.

## Drift types
- `signature-drift` — same name in header & code, type-level signature differs
  (parameter names / whitespace are NOT drift; types / const / return / arity are).
- `contract-only` — declared in the header, not implemented.
- `code-only` — implemented (non-static) but not declared.
`static` functions are private and excluded from the public contract.

## Sanity (anti-false-success on the tool itself)
`manifest.json` `expect` lists known public symbols the extractor MUST recover;
if it can't, the region is reported BROKEN, not "clean" — so an empty parse can
never masquerade as drift 0.

## Use
```
make conform                 # sweep all regions (CI / manual)
python3 conform/check.py --file <path>   # one region (hook mode)
```
Loop state: `slice1-v2/.tmp/conform-state.json` (gitignored).
