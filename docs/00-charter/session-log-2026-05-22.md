# 세션 진행 보고서 #1 — 2026-05-22

**세션 유형:** 방법론 응용 연구 + 조직/프로세스 설계
**참여자:** luke (HU), Opencode/GLM (OC-orchestrator), Codex/GPT-5.4 (CX), Gemini (GE)
**상태:** Phase 0 완료, Phase A-1 진입 대기

---

## 세션 타임라인

### [T1] 프로젝트 확인

luke가 alpha-n-naia-144 프로젝트 확인 요청. 프로젝트 구조 파악:
- 한글 자모 타일 게임 + 자체 터미널/폰트 렌더 엔진
- 1.44MB 디스켓, 공모전 마감 2026-09-04
- 현재 상태: Gate 0 Charter (사용자 검토 대기), 소스코드 없음
- 커밋 1개 (초기 헌장)

### [T2] 방법론 전환 논의

luke: "요구사항을 발의했는데 원래는 SDLC를 하는게 목적이었거든? 이번엔 좀 다르게 해볼까 해"

→ SDLC 문서 게이트 방식 대신 새로운 접근을 모색

### [T3] 역할별 에이전트 도입

luke: "역할의 에이전트를 두자"

→ 초기 제안: Architect, Engineer, Designer, Writer, Builder, Reviewer (6개)
→ luke 반응: AI의 실수확률 ~20%를 고려하면 소수 에이전트는 망함

### [T4] 핵심 통찰 — 20% 환각과 다수 보팅

luke가 설명한 구조:
- AI 환각/실수 확률 ~20%
- 에이전트 수가 적으면 책임 과중 → 단일 장애점
- **다수 에이전트가 동일 작업 독립 수행 → 보팅 → 오류 분산**
- **적대적 크로스리뷰 필요**

### [T5] 특허 연결

luke: "그래서 내가 특허를 낸게 있었어"

→ 특허 10-2026-0056403 "멀티에이전트 상호 검증 프레임워크" 확인
→ 이 게임 프로젝트 = 특허 방법론의 **실증 실험장**

### [T6] 실험 원칙 확정

luke: "보통은 내가 어느정도 방향을 잡는데 이번엔 안해볼려고. 뭐가 나오는지에 대한 실험"

확정된 원칙:
1. **제1원칙: 기록한다** — 활동 자체가 보고서
2. 방향은 luke가 안 줌, 에이전트 자율
3. 제약만 지킴: 1.44MB, 한글 자모 타일, 공모전 마감

### [T7] 리소스 제약

luke: "gemini cli, codex cli, claude는 이번주 다썼음"

→ 가용: Gemini CLI 0.42.0, Codex CLI 0.130.0, Opencode/GLM
→ Claude 제외

### [T8] 3-AI 교차 검증 실행

OC가 연구 문서 작성 → Codex + Gemini에 독립 리뷰 요청:

**Codex 응답 요약:**
- 최대 리스크: 다중 생성이 아키텍처 일관성과 용량 예산 파괴
- 생성 확장: 생산 계약 + 좁은 단위 + merge arbiter 필요
- 최소 에이전트: 5 (실용적 하한), 7 (안정)
- Top 3: 안티앵커링, 건강 점수, 스트라이크/퇴출

**Gemini 응답 요약:**
- 최대 리스크: 산출물 파편화 → 통합 실패
- 생성 확장: 평가 보팅 + 모델 계층화
- 최소 에이전트: R=3 (최소)
- Top 3: 스트라이크 관리, 안티앵커링, 컨텍스트 드리프트

**공통 합의 (2/2):**
1. 생성에는 보팅이 아니라 "생성 계약"이 먼저
2. 좁은 산출물 단위로 쪼개서 생성
3. 안티앵커링 최우선
4. 3개로 시작, 5개로 확장

### [T9] 산출물 작성

3개 문서 생성:
1. `methodology-research.md` — 방법론 연구 + 교차 검증 결과
2. `agent-organization.md` — 조직 구성
3. `process-and-deliverables.md` — 프로세스와 산출물

---

## 산출물 인덱스

| 파일 | 역할 | 최초 작성 |
|------|------|----------|
| `docs/00-charter/methodology-research.md` | 방법론 갭 분석, 3-AI 교차 검증, 종합 판단 | 2026-05-22 |
| `docs/00-charter/agent-organization.md` | 에이전트 풀, 생성 계약, 역할 매트릭스 | 2026-05-22 |
| `docs/00-charter/process-and-deliverables.md` | 워크플로우, 모듈 순서, 라운드 프로토콜 | 2026-05-22 |
| `docs/00-charter/session-log-2026-05-22.md` | 본 문서 — 세션 전체 흐름 기록 | 2026-05-22 |

기존 문서 (Gate 0 SDLC):
| `docs/00-charter/charter.md` | 프로젝트 헌장 | 2026-05-21 |
| `docs/00-charter/ai-roles.md` | 기존 AI 역할 분담 (SDLC 기반) | 2026-05-21 |
| `docs/00-charter/sdlc-process.md` | SDLC 게이트 정의 | 2026-05-21 |

---

## 결정 이력

| # | 날짜 | 결정 | 근거 | 결정자 | 비고 |
|---|------|------|------|--------|------|
| D1 | 2026-05-22 | SDLC 대신 특허 방법론 응용 실험으로 전환 | luke 방향 제시 | HU | |
| D2 | 2026-05-22 | 활동 자체를 보고서로 기록 | 제1원칙 | HU | 모든 논의/결정 기록 |
| D3 | 2026-05-22 | Claude 제외, Gemini+Codex로 우선 진행 | Claude 주간 사용량 소진 | HU | |
| D4 | 2026-05-22 | 사용자 개입 최소화 | 자율 실험 | HU | 방향 미제공 |
| D5 | 2026-05-22 | 생성에는 보팅이 아닌 '생성 계약' 우선 | 3-AI 교차 합의 | OC+CX+GE | Codex, Gemini 모두 동의 |
| D6 | 2026-05-22 | 좁은 산출물 단위로 모듈 분리 | 3-AI 교차 합의 | OC+CX+GE | 전체 게임 통째 생성 금지 |
| D7 | 2026-05-22 | 안티앵커링 최우선 구현 | 3-AI 교차 합의 | OC+CX+GE | 양쪽 모두 필수 지목 |
| D8 | 2026-05-22 | 3 에이전트로 시작, 5로 확장 | Codex(5 하한) + Gemini(3 최소) 타협 | OC | |

### [T10] 추가 맥락 — 세계관, 텍스트 선택 이유, 세계관 자료 위치

luke가 세 가지 추가 정보 제공:

1. **cafelua + naia 세계관 존재**: 이미지 기반 세계관이 이미 존재함. 콘텐츠 기획(시나리오, 캐릭터, 맵) 시 참고해야 함.
   - cafelua 공간 설정: `projects/cafelua.com/data/cafelua/design/cafelua_space_concept.md`
   - Alpha(메이드 AI) 외형: `projects/cafelua.com/data/alpha/design/alpha-appearance-description.ko.md`
   - naia 브랜드 페이지: `projects/naia.nextain.io/src/app/[lang]/(public)/naia/page.tsx`
   - naia 캐릭터 이미지: `projects/naia.nextain.io/public/branding/character/`
   - naia i18n 데이터: `projects/naia.nextain.io/src/i18n/dictionaries/ko.ts` → `naia` 섹션

2. **텍스트 선택 이유**: 1.44MB 안에서 텍스트를 주 매체로 한 것은 사이즈 때문만이 아님. **AI가 읽고 생성하고 검증하기 가장 좋은 포맷이 텍스트**이기 때문. AI가 자율 루프를 돌려면 산출물을 직접 읽고 판단할 수 있어야 함.

3. **세계관 자료 위치**: cafelua.com/data/ 와 naia.nextain.io/ 브랜드 페이지에 위치함.

---

## 다음 단계

Phase A-1: 생성 계약 정의
- `docs/contracts/size-budget.md` — 사이즈 예산 상세
- `docs/contracts/interfaces.md` — 모듈 간 인터페이스
- `docs/contracts/tile-spec.md` — 한글 자모 타일 규격
- `docs/contracts/tech-stack.md` — 언어/플랫폼 결정

절차: OC 초안 → CX + GE 독립 리뷰 → 투표 → 확정
