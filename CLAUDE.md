# AGENTS.md — alpha-n-naia-144

> **이 파일은 `CLAUDE.md` · `GEMINI.md` 와 동일 내용입니다.** (멀티툴 entry)

## 프로젝트 한 줄

한글 자모를 그래픽 타일로 사용하는 1.44MB 디스켓 게임 + 자체 터미널/폰트 렌더 엔진. `luke_n_alpha/alpha-n-naia-144` 개인 프로젝트.

## Mandatory Reads (세션 시작마다)

1. `docs/00-charter/charter.md` — 프로젝트 헌장 (비전·범위·제약)
2. `docs/00-charter/ai-roles.md` — AI 역할 분담 + RACI
3. `docs/00-charter/sdlc-process.md` — SDLC 게이트 정의·검토 절차
4. `.agents/context/agents-rules.json` — 운영 규칙 (SoT)
5. `.agents/context/project-index.yaml` — 컨텍스트 인덱스
6. **현재 단계의 `docs/0N-*/README.md`** — 진행 중인 SDLC 단계

## 핵심 원칙

- **SDLC 철저 준수**: 단계별 산출물 → 사용자 검토 게이트 → 다음 단계. 게이트 우회 금지.
- **멀티 AI 앙상블**: cross-review/judge는 단일 AI 외주 금지. Codex/Gemini/GLM/Opencode 다중 호출이 디폴트 ([[feedback_pi_substrate_not_glm_only_2026_05_20]]).
- **AI 리드 · 사람 실행**: 코드 작성·로컬 커밋은 AI OK. push·외부 배포·공모전 제출은 사용자(luke) ([[feedback_ai_leads_human_executes_serverenv]]).
- **1.44MB 예산**: 모든 결정은 사이즈 예산(`docs/02-design/size-budget.md`)에 항목 기재 후 진행.
- **오픈소스 메타 공개**: 결정·실패·되돌리기 모두 commit 히스토리와 progress 보고서에 정직하게 남긴다. 사후 미화 금지.

## 작업 흐름

```
요구사항/설계 변경 요청
  → 해당 SDLC 단계 문서 초안 작성 (AI)
  → 멀티 AI cross-review (cross-tool ensemble)
  → 통합 (Claude 오케스트레이터)
  → 사용자 검토 게이트 (luke)
  → 승인 시 commit + 다음 단계
  → 반려 시 피드백 반영 후 재제출
```

## 산출물 라이프사이클

- 진행 중: `docs/0N-*/`
- 단계별 진행 보고서: `.agents/progress/<topic>-YYYY-MM-DD.md`
- 단계 종결 시: `.agents/progress/archive/YYYY-MM/` 이동
- 임시 스크립트: `.agents/work/` 또는 `tmp/` (둘 다 gitignored)

## 컨텍스트 미러

- `.agents/` = AI SoT (JSON/YAML/EN-friendly 기술 문서)
- `.users/ko/` = 사람용 한국어 mirror (수동 동기화, 자동 mirror 없음)
- `docs/` = SDLC 산출물 (한국어 우선 — 사용자 검토 우선)

본 entry 파일 3종(`AGENTS.md` · `CLAUDE.md` · `GEMINI.md`)은 항상 동일 내용 유지. 수정 시 3파일 모두.
