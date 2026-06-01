#!/usr/bin/env python3
"""Deterministic contract<->code conformance gate for naia-144 (no LLM, 0 tokens).

Two modes:
  --all          check every region; human report; exit 1 if drift/broken.
  --file <path>  check the region(s) that <path> belongs to; update loop state;
                 print a JSON verdict for the edit-time hook.

Why: an AI working uncommitted can loop for hours building on a DRIFTED contract.
This gate runs after each edit (free), and when contract<->code diverge it tells
the agent to STOP and reconcile — and if the same region stays drifted across N
edits (flailing), it escalates to a halt. The judge is the deterministic oracle
(generation != judging); a per-region SANITY check ensures the extractor really
extracts, so "drift 0" can never be a false success from an empty parse.
"""
from __future__ import annotations

import json
import os
import re
import sys

_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, _HERE)
import oracle  # noqa: E402

BASE = os.path.dirname(_HERE)                     # slice1-v2/
MANIFEST = os.path.join(_HERE, "manifest.json")
STATE = os.path.join(BASE, ".tmp", "conform-state.json")
HALT_N = 3                                        # consecutive drifted edits → halt

_FUNC = re.compile(
    r"^[ \t]*(static\s+)?"
    r"([A-Za-z_][\w \t\*]*?[\w\*])[ \t]+"
    r"([A-Za-z_]\w*)[ \t]*"
    r"\(([^;{]*)\)[ \t]*"
    r"([;{])",
    re.MULTILINE,
)
_SKIP = {"if", "for", "while", "switch", "return", "sizeof", "typedef", "struct"}


def extract(path: str, want: str) -> list[dict]:
    out, seen = [], set()
    try:
        text = open(path, encoding="utf-8").read()
    except OSError:
        return out
    for m in _FUNC.finditer(text):
        is_static, ret, name, params, end = (
            m.group(1), m.group(2).strip(), m.group(3), m.group(4).strip(), m.group(5))
        if name in _SKIP:
            continue
        if want == "proto" and end != ";":
            continue
        if want == "def_public":
            if end != "{" or is_static:           # static = private, not in public contract
                continue
        if name in seen:
            continue
        seen.add(name)
        out.append({"name": name, "sig": f"{ret} {name}({params})"})
    return out


def load_manifest() -> dict:
    with open(MANIFEST, encoding="utf-8") as f:
        return json.load(f)


def check_region(reg: dict) -> dict:
    """Return {drift, broken, contract, code} for one region."""
    contract = extract(os.path.join(BASE, reg["header"]), "proto")
    code = extract(os.path.join(BASE, reg["source"]), "def_public")
    found = {s["name"] for s in contract} | {s["name"] for s in code}
    broken = [e for e in reg.get("expect", []) if e not in found]   # sanity: extractor must work
    drift = {} if broken else oracle.drift_set_typelevel(contract, code)
    return {"drift": drift, "broken": broken, "contract": contract, "code": code}


def regions_for_file(path: str, man: dict) -> list[dict]:
    b = os.path.basename(path)
    return [r for r in man["regions"] if b in (r["header"], r["source"])]


def _load_state() -> dict:
    try:
        with open(STATE, encoding="utf-8") as f:
            return json.load(f)
    except (OSError, ValueError):
        return {}


def _save_state(st: dict) -> None:
    os.makedirs(os.path.dirname(STATE), exist_ok=True)
    with open(STATE, "w", encoding="utf-8") as f:
        json.dump(st, f, indent=1)


def mode_all() -> int:
    man = load_manifest()
    bad = 0
    print("conform gate — all regions")
    for reg in man["regions"]:
        r = check_region(reg)
        if r["broken"]:
            bad += 1
            print(f"  [BROKEN] {reg['name']}: extractor missed {r['broken']} — UNTRUSTWORTHY")
        elif r["drift"]:
            bad += 1
            print(f"  [DRIFT ] {reg['name']}: {r['drift']}")
        else:
            print(f"  [clean ] {reg['name']} ({len(r['contract'])} public fn)")
    print(f"=> {'FAIL' if bad else 'PASS'} ({bad} region(s) need attention)")
    return 1 if bad else 0


def mode_file(path: str) -> int:
    man = load_manifest()
    regs = regions_for_file(path, man)
    if not regs:
        print(json.dumps({"status": "skip"}))       # not a conform-tracked file
        return 0
    st = _load_state()
    drifted, broken_regs, details = {}, [], {}
    for reg in regs:
        r = check_region(reg)
        if r["broken"]:
            broken_regs.append(reg["name"])
            st[reg["name"]] = st.get(reg["name"], 0) + 1
            details[reg["name"]] = {"broken": r["broken"]}
        elif r["drift"]:
            st[reg["name"]] = st.get(reg["name"], 0) + 1
            drifted[reg["name"]] = st[reg["name"]]
            details[reg["name"]] = r["drift"]
        else:
            st[reg["name"]] = 0                       # reconciled → reset loop counter
    _save_state(st)

    issues = {**{k: v for k, v in drifted.items()}}
    if not issues and not broken_regs:
        print(json.dumps({"status": "clean"}))
        return 0
    maxcount = max([st[r] for r in list(drifted) + broken_regs], default=1)
    halt = maxcount >= HALT_N
    lines = []
    for name in broken_regs:
        lines.append(f"{name}: extractor BROKEN (cannot verify — fix the parser/contract)")
    for name, cnt in drifted.items():
        lines.append(f"{name}: {details[name]}  (drifted for {cnt} edit(s))")
    body = "; ".join(lines)
    if halt:
        msg = (f"[conform] HALT — contract<->code drift unresolved for {maxcount} edits: {body}. "
               f"STOP adding code. Reconcile the contract (declare/implement/remove) or escalate "
               f"to a human BEFORE any further edits — you are looping on a drifted base.")
        print(json.dumps({"status": "halt", "message": msg, "regions": details}))
    else:
        msg = (f"[conform] DRIFT — {body}. Reconcile contract<->code before adding more "
               f"(this is the daf35d0 false-success class). Fix now, don't build on it.")
        print(json.dumps({"status": "drift", "message": msg, "regions": details}))
    return 0


def main() -> int:
    args = sys.argv[1:]
    if args and args[0] == "--all":
        return mode_all()
    if len(args) >= 2 and args[0] == "--file":
        return mode_file(args[1])
    print("usage: check.py --all | --file <path>", file=sys.stderr)
    return 2


if __name__ == "__main__":
    sys.exit(main())
