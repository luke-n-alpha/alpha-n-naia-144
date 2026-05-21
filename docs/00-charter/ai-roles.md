# AI 역할 분담 + RACI — alpha-n-naia-144

> **⚠ DEPRECATED (2026-05-22)**: 본 문서는 SDLC 게이트 방식 기반 역할 분담. 방법론 전환 이후 `agent-organization.md`로 대체. 새 에이전트 풀(OC/CX/GE)과 퇴출 규칙은 해당 문서 참조.

**문서 상태:** Deprecated (SDLC → 특허 방법론 전환)
**작성일:** 2026-05-21
**기반 원칙:** 멀티 AI 앙상블 ([[feedback_pi_substrate_not_glm_only_2026_05_20]]), AI 리드 · 사람 실행 ([[feedback_ai_leads_human_executes_serverenv]])

---

## 1. 참여자

### 사람

| ID | 이름 | 역할 |
|---|---|---|
| HU | luke (사용자) | 결재자 (Approver), 비전 제시자, 외부 실행자 (push/deploy/제출) |

### AI

| ID | 도구 | 주특기 | 주 역할 |
|---|---|---|---|
| CL | **Claude** (Opus, 본 세션) | 멀티 영역 통합, 긴 컨텍스트 일관성 유지 | **오케스트레이터** — 단계 진행 리드, 문서 통합, 충돌 조정 |
| CX | **Codex** (GPT-5) | 시스템 SW, C/Zig, 비트맵 알고리즘, 성능 최적화 | **시스템 아키텍트** — 엔진 코어 설계 검토, 성능 분석 |
| GE | **Gemini** | 시각·UX 비평, 멀티모달, 게임 디자인 | **디자인/UX 비평가** — 한글 자모 매핑 시각 검증, 게임플레이 비평, 터미널 호환성 |
| GL | **GLM** | 한국어 텍스트 자연성, narrative balance | **시나리오 검토** — 한국어 대화·서사 검토, NPC 대사 품질 |
| OC | **Opencode** | 빌드 시스템, 정적 분석, 리팩토링 | **빌드 엔지니어** — 1.44MB 사이즈 최적화, 컴파일 옵션, dead code 제거 |

> **원칙**: 모든 cross-review는 **최소 2개 이상의 AI**가 참여. 단일 AI 외주 금지 ([[feedback_pi_substrate_not_glm_only_2026_05_20]]).

## 2. 의사결정 흐름 (Decision Flow)

```
사용자(HU) 비전/요청
   ↓
오케스트레이터(CL) 단계 산출물 초안 작성
   ↓
영역별 전문 AI(CX/GE/GL/OC) 병렬 리뷰
   ↓
오케스트레이터(CL) 의견 통합 + 통합 산출물
   ↓
사용자(HU) 최종 검토 게이트
   ↓
   ├─ 승인 → commit → 다음 단계
   └─ 반려 → 피드백 반영 → 재제출
```

## 3. RACI 매트릭스 (단계별)

> **R** = Responsible (실행) · **A** = Accountable (책임/승인) · **C** = Consulted (자문) · **I** = Informed (통보)

### Gate 0 — Charter

| 작업 | HU | CL | CX | GE | GL | OC |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| 헌장·역할·프로세스 초안 작성 | A | R | C | C | I | I |
| 비전·범위 승인 | **A** | I | I | I | I | I |

### Gate 1 — Requirements

| 작업 | HU | CL | CX | GE | GL | OC |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| 기능 요구사항 작성 | A | R | C | C | I | I |
| 비기능 요구사항 (사이즈/성능) | A | R | C | I | I | **C** |
| 사용자 시나리오 (게임 흐름) | A | R | I | **C** | **C** | I |
| 공모전 규정 재확인 | A | R | I | I | I | I |
| 라이선스 결정 | **A** | R | I | I | I | I |

### Gate 2 — Design

| 작업 | HU | CL | CX | GE | GL | OC |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| 아키텍처 (4계층) | A | R | **C** | C | I | C |
| 한글 자모 매핑 알고리즘 | A | R | **C** | **C** | I | I |
| 터미널 엔진 명세 | A | R | **C** | C | I | C |
| 폰트 렌더러 명세 | A | R | **C** | **C** | I | I |
| 듀얼 렌더 파이프라인 | A | R | **C** | C | I | C |
| 사이즈 예산 (1.44MB 분배) | A | R | C | I | I | **C** |
| 게임 디자인 (서사·NPC·룰) | A | R | I | **C** | **C** | I |
| 캐릭터 (cafelua·naia 고양이) | **A** | R | I | **C** | **C** | I |
| PoC (자모 매핑 정지 frame 5장) | A | R | C | **C** | I | I |

### Gate 3 — Engine Core 구현

| 작업 | HU | CL | CX | GE | GL | OC |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| 코드 작성 (engine) | A | R | **C** | I | I | C |
| 코드 리뷰 (성능·정확성) | A | R | **C** | I | I | **C** |
| 사이즈 측정 + 예산 추적 | A | R | I | I | I | **C** |
| 시각 검증 (PoC 재실행) | A | R | C | **C** | I | I |

### Gate 4 — Game Content

| 작업 | HU | CL | CX | GE | GL | OC |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| 시나리오 작성 | A | R | I | C | **C** | I |
| NPC 룰베이스 AI | A | R | **C** | C | **C** | I |
| 챕터 구현 | A | R | C | C | C | I |
| 픽셀 모드 데모 챕터 | A | R | **C** | **C** | I | C |
| 게임플레이 비평 | A | R | C | **C** | **C** | I |

### Gate 5 — Testing & Size

| 작업 | HU | CL | CX | GE | GL | OC |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| 단위 테스트 | A | R | **C** | I | I | C |
| 통합 테스트 | A | R | **C** | C | I | C |
| 시각 회귀 테스트 | A | R | C | **C** | I | I |
| 사이즈 예산 close | A | R | I | I | I | **C** |
| 시연 영상 (시각) | **A** | R | I | **C** | I | I |

### Gate 6 — Deployment

| 작업 | HU | CL | CX | GE | GL | OC |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| 디스켓 이미지 빌드 | A | R | C | I | I | **C** |
| 한·영 README 정비 | A | R | I | C | C | I |
| GitHub push | **R/A** | C | I | I | I | I |
| 공모전 제출 | **R/A** | C | I | I | I | I |

## 4. 멀티 AI 호출 패턴

### 4.1 Cross-review (단계 초안 → 검토)

```
CL 초안 → 동시 호출:
  CX (시스템/성능)
  GE (시각/UX)
  GL (한국어 narrative)   ← Gate 1/4에서 필수
  OC (사이즈/빌드)         ← Gate 2/3/5/6에서 필수
→ CL 통합 → HU 검토
```

### 4.2 Judge (충돌 시 결정 보조)

- AI 간 의견 충돌 시 **추가 1개 AI**를 judge로 호출.
- judge의 판단은 권고일 뿐, **최종 결정은 HU**.

### 4.3 호출 도구

- pi 핀번들 substrate가 가용하면 우선 ([[project_naia_own_orchestrator_pi_substrate]]).
- 미가용 시 subagent-CLI 직접 호출 (codex/gemini/glm/opencode CLI).

## 5. 산출물 저장

- **단계별 산출물**: `docs/0N-*/`
- **AI 리뷰 raw 출력 (영구 보존 필요 시)**: `.agents/progress/reviews/<단계>-<날짜>-<AI>.md`
- **임시 산출물**: `.agents/work/` (gitignored)
- **합의·결정 기록**: `.agents/progress/decisions-YYYY-MM-DD.md`

## 6. 충돌·이의 처리

- AI 간 충돌: §4.2 judge → HU 최종 결정.
- HU vs AI 충돌: **HU 우선**. AI는 근거 제시 후 따른다.
- 단, 사이즈/규정 위반 같은 **객관 제약 위반**은 AI가 우선 차단 (HU에게 보고 후 진행 여부 결정).

## 7. 본 문서 개정

본 역할표는 실제 운용에서 비효율 발견 시 즉시 개정 (드래프트 → HU 검토 → 통과 → 버전 증가).
