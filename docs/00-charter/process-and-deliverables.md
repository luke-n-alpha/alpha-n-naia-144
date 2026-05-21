# 프로세스와 산출물 구성 — alpha-n-naia-144

**작성일:** 2026-05-22
**상태:** 설계안 (3-AI 교차 검증 완료)
**기반:** methodology-research.md, agent-organization.md

---

## 1. 전체 워크플로우

```
┌─────────────────────────────────────────────────────┐
│                생성 계약 정의 (OC)                    │
│  사이즈 예산, 기술 제약, 인터페이스 계약, 타일 규격    │
└──────────────────────┬──────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────┐
│            모듈 단위 생성 라운드 (반복)                │
│                                                       │
│  Step 1: 생성 (CX, GE 독립)                          │
│    좁은 산출물 단위로 각 에이전트가 독립 생성          │
│                                                       │
│  Step 2: 제약 검사 (OC)                               │
│    사이즈 예산, 인터페이스 계약 준수 자동 검사         │
│                                                       │
│  Step 3: 후보 병합 (OC = merge arbiter)               │
│    여러 안에서 채택 가능한 부분만 조합                 │
│    충돌 시 상위 판단으로 해소                          │
│                                                       │
│  Step 4: 다중 검증 (특허 메커니즘)                    │
│    Phase 1: 독립 리뷰 (CX, GE 격리)                   │
│    Phase 2: 투표 (CONFIRMED/CONTESTED/DISMISSED)      │
│    Phase 3: 선택적 토론 (CONTESTED만)                 │
│    + 건강 점수 + 스트라이크 + 드리프트 감지            │
│                                                       │
│  Step 5: 수렴 판정                                    │
│    연속 2회 CLEAN → 모듈 완료                         │
│    CONFIRMED 발견사항 → 수정 후 다음 라운드            │
└──────────────────────┬──────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────┐
│              통합 빌드 + 사이즈 검증                   │
│  전체 1.44MB 예산 재확인, 통합 테스트                  │
└──────────────────────┬──────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────┐
│              공모전 제출 (HU 실행)                     │
└─────────────────────────────────────────────────────┘
```

---

## 2. 산출물 목록

### 2.1 메타 산출물 (방법론 자체의 산출물)

| 산출물 | 위치 | 설명 |
|--------|------|------|
| 이벤트 로그 | `logs/events.jsonl` | 모든 라운드의 이벤트 기록 (JSONL 추가 전용) |
| 건강 점수 이력 | `logs/health-scores.jsonl` | 에이전트별 라운드별 건강 점수 |
| 스트라이크 이력 | `logs/strikes.jsonl` | 스트라이크 발생/면제 기록 |
| 투표 기록 | `logs/votes.jsonl` | 모든 투표 결과와 근거 |
| 생성 계약 | `docs/contracts/` | 각 모듈의 생성 전 계약 문서 |
| 라운드 보고서 | `docs/rounds/` | 각 라운드의 종합 보고서 |

### 2.2 게임 산출물 (프로젝트 자체의 산출물)

| 산출물 | 위치 | 담당 |
|--------|------|------|
| 터미널 엔진 | `src/engine/terminal/` | CX |
| 폰트 렌더러 | `src/engine/font/` | CX |
| 렌더 파이프라인 | `src/engine/render/` | CX |
| 한글 자모 타일셋 | `src/assets/tiles/` | GE |
| 게임 월드/맵 | `src/game/world/` | CX + GE |
| 시나리오/대사 | `src/assets/scenarios/` | GE |
| 입력 처리 | `src/engine/input/` | CX |
| 사운드 | `src/assets/sfx/` | CX |
| 빌드 스크립트 | `tools/` | CX |
| 사이즈 예산 추적 | `docs/contracts/size-budget.md` | OC |

---

## 3. 모듈 생성 순서

의존성에 따라 아래 순서로 진행:

```
Phase A: 기반 인프라
  A1. 생성 계약 정의 (사이즈 예산, 인터페이스)
  A2. 한글 자모 타일셋 + 밝기 매핑 (GE 생성 → CX 검증)
  A3. 폰트 렌더러 (CX 생성 → GE 시각 검증)

Phase B: 엔진 코어
  B1. 터미널 엔진 (CX 생성 → GE 검증)
  B2. 듀얼 렌더 파이프라인 (CX 생성 → GE 검증)
  B3. 입력 처리 (CX 생성 → CX 자체 + OC 검증)

Phase C: 게임 콘텐츠
  C1. 월드/맵 구조 (CX + GE 독립 생성 → OC 병합)
  C2. 시나리오/NPC (GE 생성 → OC 검증)
  C3. 게임 로직 (CX 생성 → GE + OC 검증)

Phase D: 통합
  D1. 전체 통합 빌드
  D2. 사이즈 예산 close (1.44MB 확인)
  D3. 디스켓 이미지 생성
```

---

## 4. 라운드 프로토콜 (모듈 단위)

각 모듈 생성 시 다음 프로토콜 적용:

### 4.1 생성 (Step 1)

```
입력: 생성 계약 (제약, 인터페이스, 도메인)
실행: 담당 에이전트가 독립 생성
출력: 산출물 + 자체 files_read 목록 + 생성 노트
```

핵심 생성 에이전트가 2개인 경우 (CX + GE):
- 각각 독립적으로 동일 모듈을 생성
- 서로 다른 접근 가능 (예: CX는 성능 우선, GE는 가독성/디자인 우선)

### 4.2 제약 검사 (Step 2)

오케스트레이터(OC)가 자동 검사:
- 사이즈 예산 준수 여부
- 인터페이스 계약 준수 여부
- 타일 규격 / 인코딩 준수 여부

위반 시: 해당 에이전트에게 피드백 후 재생성

### 4.3 후보 병합 (Step 3)

복수 생성 결과가 있을 경우:
- OC가 merge arbiter로 동작
- 각 후보의 장단점을 평가
- 채택 가능한 부분만 조합
- 충돌 시: 오케스트레이터 판단

### 4.4 다중 검증 (Step 4)

특허 메커니즘 적용:

```
Phase 1 (독립 리뷰):
  R명 검증 에이전트가 격리 컨텍스트에서 병렬 리뷰
  각각 구조화된 보고서 출력:
    - Scope, Files read, Findings (심각도+위치+증거), Verdict

Phase 2 (투표):
  발견사항 매칭 (코드 위치 오버랩)
  → CONFIRMED / CONTESTED / AUTO-DISMISSED
  건강 점수 산출 (7-신호)
  스트라이크 누적/면제 (도메인 인지)

Phase 3 (선택적 토론):
  CONTESTED 발견사항만
  오케스트레이터가 교차 심문
  최대 3 서브라운드
  최종 투표 → 확인 또는 기각
```

### 4.5 수렴 판정 (Step 5)

- 연속 2회 CLEAN 라운드 → 모듈 완료
- CONFIRMED 발견사항 존재 → 수정 후 다음 라운드
- 최대 10라운드 → 초과 시 사용자 개입

---

## 5. 이벤트 로그 스키마

```jsonl
{"type":"ROUND_STARTED","module":"font-renderer","round":1,"timestamp":"..."}
{"type":"AGENT_SPAWNED","agent":"cx","role":"creator","domain":"engine","timestamp":"..."}
{"type":"ARTIFACT_CREATED","agent":"cx","module":"font-renderer","files":["src/engine/font/"],"size_bytes":12345,"timestamp":"..."}
{"type":"CONSTRAINT_CHECK","module":"font-renderer","passed":true,"violations":[],"timestamp":"..."}
{"type":"REVIEW_STARTED","module":"font-renderer","round":1,"reviewers":["ge","oc"],"timestamp":"..."}
{"type":"REPORT_SUBMITTED","reviewer":"ge","verdict":"FOUND_ISSUES","finding_count":3,"timestamp":"..."}
{"type":"FINDING_CONFIRMED","finding":"f1","severity":"HIGH","voters":["ge","oc"],"timestamp":"..."}
{"type":"HEALTH_SCORE","reviewer":"ge","round":1,"score":78,"signals":{...},"timestamp":"..."}
{"type":"ROUND_COMPLETED","module":"font-renderer","round":1,"confirmed":2,"clean":false,"timestamp":"..."}
{"type":"MODULE_COMPLETED","module":"font-renderer","total_rounds":3,"timestamp":"..."}
```

---

## 6. 첫 실행 계획

### Phase A-1: 생성 계약 정의

이것이 이 프로젝트의 첫 산출물이 됨.

| 항목 | 내용 |
|------|------|
| 사이즈 예산 | docs/contracts/size-budget.md |
| 인터페이스 계약 | docs/contracts/interfaces.md |
| 타일 규격 | docs/contracts/tile-spec.md |
| 기술 스택 결정 | docs/contracts/tech-stack.md |

이 계약들은 OC가 초안 작성 → CX + GE 독립 리뷰 → 투표 → 확정.
