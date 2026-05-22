#!/usr/bin/env python3
"""Harness Runtime — 에이전트 조직 런타임.

프로세스를 코드로 강제한다:
- 게이트 통과 없이 다음 phase 잠금
- 역할 위반 감지
- 회고 없이 phase 종료 불가
- human 승인 없이 인도 불가
"""

import json
import os
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).parent.parent
STORE = ROOT / ".harness" / "store.json"
LOG_DIR = ROOT / ".harness" / "logs"
ROLES_DIR = Path(__file__).parent / "roles"

AGENTS = {
    "OC": {"model": "opencode", "cmd": "opencode"},
    "CX": {"model": "codex", "cmd": "codex"},
    "GE": {"model": "gemini", "cmd": "gemini"},
}

PHASES = [
    {
        "id": "phase_0",
        "name": "contract_definition",
        "agent": "OC",
        "gate": lambda: _check_files("docs/contracts/tech-stack.md", "docs/contracts/interfaces.md"),
        "retrospective": True,
    },
    {
        "id": "phase_a1",
        "name": "first_module_generation",
        "agents": ["CX", "GE"],
        "mode": "independent",
        "gate": lambda: _count_files("docs/candidates") >= 2,
        "retrospective": True,
    },
    {
        "id": "phase_a2",
        "name": "merge_and_review",
        "agent": "OC",
        "gate": lambda: _latest_review_pass(),
        "retrospective": True,
    },
    {
        "id": "phase_b",
        "name": "engine_core",
        "agent": "CX",
        "reviewer": "GE",
        "gate": lambda: _latest_review_no_critical(),
        "retrospective": True,
    },
    {
        "id": "phase_c",
        "name": "game_content",
        "agent": "GE",
        "reviewer": "CX",
        "gate": lambda: _latest_review_no_critical(),
        "retrospective": True,
    },
    {
        "id": "phase_d",
        "name": "integration",
        "agent": "OC",
        "gate": lambda: _tests_pass(),
        "retrospective": True,
        "requires_human_approval": True,
    },
]


def _check_files(*paths):
    return all((ROOT / p).exists() for p in paths)


def _count_files(directory):
    d = ROOT / directory
    if not d.exists():
        return 0
    return len(list(d.iterdir()))


def _latest_review_pass():
    reviews = sorted((ROOT / "docs" / "rounds").glob("review-*.md"))
    if not reviews:
        return False
    content = reviews[-1].read_text()
    return "PASS" in content and "REJECT" not in content


def _latest_review_no_critical():
    reviews = sorted((ROOT / "docs" / "rounds").glob("review-*.md"))
    if not reviews:
        return False
    content = reviews[-1].read_text()
    return "CRITICAL" not in content


def _tests_pass():
    result = subprocess.run(["make", "test"], capture_output=True, text=True, cwd=ROOT)
    return result.returncode == 0


def _load_store():
    if STORE.exists():
        return json.loads(STORE.read_text())
    return {"current_phase": 0, "completed": [], "log": []}


def _save_store(store):
    STORE.parent.mkdir(parents=True, exist_ok=True)
    STORE.write_text(json.dumps(store, indent=2, ensure_ascii=False))


def _log(store, phase_id, event, detail):
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    entry = {
        "ts": datetime.now().isoformat(),
        "phase": phase_id,
        "event": event,
        "detail": detail,
    }
    store["log"].append(entry)
    (LOG_DIR / f"{phase_id}.jsonl").open("a").write(json.dumps(entry, ensure_ascii=False) + "\n")
    _save_store(store)


def _check_retrospective(phase_id):
    retros = sorted((ROOT / "docs" / "rounds").glob("retrospective-*.md"))
    if not retros:
        print(f"  BLOCKED: 회고 파일이 없습니다. docs/rounds/retrospective-*.md 를 작성하세요.")
        return False
    latest = retros[-1].read_text()
    if phase_id not in latest and phase_id.replace("_", "-") not in latest:
        print(f"  BLOCKED: 최신 회고에 {phase_id} 내용이 없습니다.")
        return False
    return True


def _check_role_violation(agent_id, action):
    role_file = ROLES_DIR / f"{agent_id}.md"
    if not role_file.exists():
        return False
    content = role_file.read_text()
    if agent_id == "OC" and "코드 작성 금지" in content:
        if action == "edit_code":
            print(f"  VIOLATION: OC는 코드를 작성할 수 없습니다.")
            return True
    return False


def _dispatch(agent_id, prompt, timeout=300):
    cfg = AGENTS.get(agent_id)
    if not cfg:
        print(f"  ERROR: 알 수 없는 에이전트 {agent_id}")
        return None

    cmd = cfg["cmd"]
    if cmd == "codex":
        full_cmd = ["codex", "exec", "--full-auto", prompt]
    elif cmd == "gemini":
        full_cmd = ["gemini", "-p", prompt]
    else:
        full_cmd = [cmd, prompt]

    for attempt in range(3):
        try:
            result = subprocess.run(
                full_cmd, capture_output=True, text=True,
                timeout=timeout, cwd=ROOT
            )
            if result.returncode == 0:
                return result.stdout
            if "429" in result.stderr or "rate limit" in result.stderr.lower():
                print(f"  RETRY {attempt+1}/3: {agent_id} rate limited. 30s 대기...")
                time.sleep(30)
                continue
            print(f"  ERROR: {agent_id} 반환값 {result.returncode}")
            return None
        except subprocess.TimeoutExpired:
            print(f"  TIMEOUT: {agent_id} 응답 시간 초과")
            return None
        except FileNotFoundError:
            print(f"  ERROR: {cmd} 를 찾을 수 없습니다")
            return None

    print(f"  FALLBACK: {agent_id} 3회 실패. 다른 에이전트로 대체 필요.")
    return None


def _human_approval(phase_name):
    print(f"\n  [HUMAN APPROVAL REQUIRED] Phase '{phase_name}' 완료.")
    print(f"  산출물을 검토 후 승인해주세요.")
    answer = input("  승인하시겠습니까? (yes/no): ").strip().lower()
    return answer == "yes"


def run_phase(phase_idx=None):
    store = _load_store()
    idx = phase_idx if phase_idx is not None else store["current_phase"]

    if idx >= len(PHASES):
        print("모든 phase가 완료되었습니다.")
        return

    phase = PHASES[idx]
    print(f"\n{'='*60}")
    print(f"Phase {idx}: {phase['name']} ({phase.get('id', '')})")
    print(f"{'='*60}")

    _log(store, phase["id"], "phase_start", f"Starting {phase['name']}")

    if "agent" in phase:
        agent = phase["agent"]
        print(f"  담당: {agent}")
        if "reviewer" in phase:
            print(f"  리뷰어: {phase['reviewer']}")

    print(f"\n  게이트 확인 중...")
    gate_fn = phase.get("gate")
    if gate_fn and gate_fn():
        print(f"  게이트 이미 통과.")
    elif gate_fn:
        print(f"  게이트 미통과. 에이전트에게 작업을 디스패치합니다.")
        agent = phase.get("agent", "OC")
        prompt = _build_prompt(phase)
        result = _dispatch(agent, prompt)
        if result:
            print(f"  결과 수신 ({len(result)} chars)")
            _log(store, phase["id"], "agent_result", f"{agent} completed, {len(result)} chars")
        else:
            print(f"  에이전트 실패. 수동 개입이 필요합니다.")
            _log(store, phase["id"], "agent_failed", agent)
            return

    print(f"\n  게이트 재확인...")
    if gate_fn and not gate_fn():
        print(f"  BLOCKED: 게이트를 여전히 통과하지 못했습니다.")
        _log(store, phase["id"], "gate_blocked", "Gate check failed after agent work")
        return

    print(f"\n  회고 확인...")
    if not _check_retrospective(phase["id"]):
        _log(store, phase["id"], "retrospective_missing", "Blocked")
        return

    if phase.get("requires_human_approval"):
        if not _human_approval(phase["name"]):
            print(f"  BLOCKED: human 승인 거부.")
            _log(store, phase["id"], "human_rejected", "Approval denied")
            return

    store["completed"].append(phase["id"])
    store["current_phase"] = idx + 1
    _log(store, phase["id"], "phase_complete", "Gate passed, retrospective OK")
    _save_store(store)
    print(f"\n  Phase {phase['name']} 완료. 다음: {store['current_phase']}")


def _build_prompt(phase):
    role_file = ROLES_DIR / f"{phase.get('agent', 'OC')}.md"
    role = role_file.read_text() if role_file.exists() else ""
    return f"""[역할 정의서]
{role}

[현재 Phase]
{phase['name']} - {phase.get('id', '')}

지시사항에 따라 작업을 수행하세요. 결과를 파일로 출력하세요."""


def status():
    store = _load_store()
    print(f"현재 Phase: {store['current_phase']}")
    print(f"완료: {store['completed']}")
    for i, phase in enumerate(PHASES):
        done = phase["id"] in store["completed"]
        current = i == store["current_phase"]
        marker = "DONE" if done else ("CURRENT" if current else "PENDING")
        gate_status = ""
        if not done and phase.get("gate"):
            gate_status = " [gate: " + ("PASS" if phase["gate"]() else "BLOCKED") + "]"
        print(f"  [{marker}] {phase['name']}{gate_status}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python run.py [status|run|run --phase <idx>]")
        sys.exit(1)

    cmd = sys.argv[1]
    if cmd == "status":
        status()
    elif cmd == "run":
        phase_idx = None
        if "--phase" in sys.argv:
            phase_idx = int(sys.argv[sys.argv.index("--phase") + 1])
        run_phase(phase_idx)
    else:
        print(f"Unknown command: {cmd}")
        sys.exit(1)
