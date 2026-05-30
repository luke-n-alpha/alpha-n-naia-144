# 문서 색인 (docs)

이 디렉터리의 진입점(허브). 모든 큐레이트 문서는 여기서 도달 가능해야 한다
(문서 고립 방지 — `scripts/check-doc-graph.mjs` 가 강제). 루트 `AGENTS.md`(=CLAUDE/GEMINI/OPENCODE/CODEX)
가 프로젝트 진입점이고, 이 파일은 `docs/` 내부 색인이다.

## 표준·구조 (하네스 — harden 2026-05-30)

- [프로젝트 구조 표준](./project-structure.md) — F12/F13 루트 화이트리스트, 디렉터리 규약
- [위협 모델](./threat-model.md) — 보안 경계, 시크릿 격리(T3), 추적 금지 경로
- [LLM 역할 분담](./llm-roles.md) — 작은(라이트) 모델 ↔ 큰 모델 분담, 단일 CLI 어댑터, 검출 계층

## 헌장·방법론 (00-charter/)

> ⚠️ `charter.md` · `ai-roles.md` · `sdlc-process.md` 는 2026-05-21 SDLC 시절 문서, 방법론 전환(2026-05-22) 이후 **deprecated**. 현행 비전·제약은 `methodology-research.md` 참조 (루트 `AGENTS.md` 안내).

- [방법론 응용 연구](./00-charter/methodology-research.md) — 특허 방법론 게임개발 응용 (현행 SoT)
- [에이전트 조직](./00-charter/agent-organization.md) · [프로세스·산출물](./00-charter/process-and-deliverables.md) · [세션 로그](./00-charter/session-log-2026-05-22.md)
- [헌장(deprecated)](./00-charter/charter.md) · [AI 역할(deprecated)](./00-charter/ai-roles.md) · [SDLC 프로세스(deprecated)](./00-charter/sdlc-process.md)

## 생성 계약 (contracts/) + 게임 디자인

- [게임 디자인](./game-design.md) — 게임 개요·세계관
- [기술 스택](./contracts/tech-stack.md) · [사이즈 예산](./contracts/size-budget.md) · [인터페이스](./contracts/interfaces.md) · [타일 규격](./contracts/tile-spec.md)
- [게임 디자인 계약](./contracts/game-design.md) · [리뷰 라운드 1](./contracts/review-round-1.md)

## SDLC 단계 (01–06)

각 단계의 입력·산출물·게이트 정의:
[요구사항](./01-requirements/README.md) · [설계](./02-design/README.md) · [구현](./03-implementation/README.md) · [테스트](./04-testing/README.md) · [배포](./05-deployment/README.md) · [유지보수](./06-maintenance/README.md)

## 작업 기록 / 라운드 / 후보 (면제 dir)

연대기성 작업물은 상호 링크 의무 면제(`check-doc-graph --exempt`):
- `docs/progress/` — 날짜별 진행·검토 메모 (append-only)
- `docs/rounds/` — 라운드별 검증·회고 기록 (실패 분석·포스트모템 포함)
- `docs/candidates/` — 에이전트 독립 생성 후보물

## 주기 검증

구조·문서·미러 이탈은 결정론 스크립트가 검출한다. 마이그레이션 완료 후
`scripts/verify-watch.sh start`(또는 `cron`)로 백그라운드 주기 검증 — 검출·보고만 자동, 수정은 사람/큰 모델 게이트.

## 주기 검증

구조·문서·미러 이탈은 결정론 스크립트가 검출한다. 마이그레이션 완료 후
`scripts/verify-watch.sh start`(또는 `cron`)로 백그라운드 주기 검증 — 검출·보고만 자동, 수정은 사람/큰 모델 게이트.
자세히: [LLM 역할 분담](./llm-roles.md) 의 "디텍트 계층".
