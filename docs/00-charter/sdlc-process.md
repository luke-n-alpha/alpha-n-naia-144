# SDLC 프로세스 — alpha-n-naia-144

> **⚠ DEPRECATED (2026-05-22)**: 본 문서는 SDLC 게이트 워터폴 기반 프로세스. 방법론 전환 이후 `process-and-deliverables.md`로 대체. 새 워크플로우(생성 계약 → 독립 생성 → 다중 검증 → 수렴)는 해당 문서 참조.

**문서 상태:** Deprecated (SDLC → 특허 방법론 전환)
**작성일:** 2026-05-21
**원칙:** 규모는 작지만 SDLC를 철저히 준수. 단계 우회 금지.

---

## 1. 프로세스 모델

**경량 게이트형 워터폴 + 단계 내 반복**.

- 큰 흐름은 워터폴 (Charter → Requirements → Design → Implementation → Testing → Deployment).
- 단계 내부는 반복적 (초안 → 멀티 AI 리뷰 → 통합 → 사용자 게이트).
- 후행 단계에서 선행 단계의 결함 발견 시: **선행 문서 개정 + 버전 증가** (조용한 무시 금지).

## 2. 게이트 정의

| Gate | 명칭 | 입력 | 산출물 | 통과 기준 |
|---|---|---|---|---|
| **0** | Charter | 사용자 비전 | `docs/00-charter/{charter,ai-roles,sdlc-process}.md` | HU 비전·범위·제약·일정·역할·프로세스 승인 |
| **1** | Requirements | Charter 통과 | `docs/01-requirements/{functional,non-functional,user-scenarios,contest-rules,license}.md` | HU 요구사항 승인, 라이선스 확정 |
| **2** | Design | Requirements 통과 | `docs/02-design/{architecture,jamo-mapping,terminal-engine,font-renderer,dual-render,size-budget,game-design,characters}.md` + **PoC** | HU 설계 승인 + PoC 자모 매핑 시각 OK |
| **3** | Engine Core | Design 통과 | `src/engine/` + `docs/03-implementation/engine.md` + 단위 테스트 | 엔진 데모 실행 OK + 사이즈 예산 내 |
| **4** | Game Content | Engine Core 통과 | `src/game/` + `assets/scenarios/` + `docs/03-implementation/game.md` | MVP playable + HU 게임플레이 승인 |
| **5** | Testing & Size | Game Content 통과 | `tests/` + `docs/04-testing/{test-plan,results,size-final}.md` | 모든 테스트 통과 + 사이즈 ≤ 1,474,560 바이트 |
| **6** | Deployment | Testing & Size 통과 | `build/disk.img` + `docs/05-deployment/{disk-image,submission,readme-bilingual}.md` | HU가 공모전 제출 (실행) |
| **7** | Post-release | 제출 완료 | `docs/06-maintenance/{open-source-launch,retrospective}.md` | 오픈소스 공개 + 회고 |

## 3. 단계 내 워크플로 (모든 단계 공통)

```
1. Kickoff
   - CL이 단계 README 작성 (목표·산출물 목록·시작 체크리스트)
   - 직전 단계 산출물 재확인

2. Drafting
   - 산출물별 초안 작성 (CL 주도)
   - 필요 시 영역 AI에 부분 위임 (예: CX에 알고리즘 절 작성 위임)

3. Cross-review (필수)
   - 멀티 AI에 동시 리뷰 요청
   - raw 리뷰는 .agents/progress/reviews/ 보존

4. Integration
   - CL이 리뷰 의견 통합, 충돌은 §4.2 judge로 보조
   - 통합본 commit

5. User Gate (HU)
   - HU가 산출물 검토
   - 승인: 다음 단계 진입 신호 발부
   - 반려: 피드백 → 2번으로 돌아감

6. Closing
   - 단계 종료 시 진행 보고서 .agents/progress/<gate>-close-YYYY-MM-DD.md
   - 단계 종결 후 progress는 archive/YYYY-MM/ 이동
```

## 4. 산출물 작성 규칙

### 4.1 문서 명명

- 단계 산출물: `docs/0N-단계명/문서명.md`
- 진행 보고서: `.agents/progress/<topic>-YYYY-MM-DD.md`
- 결정 기록: `.agents/progress/decisions-YYYY-MM-DD.md`
- 리뷰 raw: `.agents/progress/reviews/<gate>-<date>-<ai-id>.md`

### 4.2 문서 헤더 (모든 SDLC 문서 필수)

```markdown
# <문서 제목>

**문서 상태:** Draft v0.N (Gate N) | Approved v1.0 (Gate N) | Superseded by ...
**작성일:** YYYY-MM-DD
**작성:** <AI> (역할) · <리뷰어 AI> 검토 · HU 승인일 YYYY-MM-DD
**언어:** 한국어 (SoT)
```

### 4.3 버전 관리

- Draft → 사용자 승인 시 `Approved v1.0`로 변경 + commit.
- 이후 변경은 Draft v1.1, Draft v1.2, … → 승인 시 Approved v1.x.
- 큰 방향 전환은 v2.0 (이전 v1.x를 `Superseded by` 명시 후 보존).

## 5. 게이트 통과 ≠ 자동 진행

- 게이트 통과는 **HU의 명시적 진입 신호**로만 다음 단계 진입.
- AI 단독 "이 정도면 됐다" 판단 금지.
- HU가 "OK 다음 단계" 또는 동등 의사 표시 시에만 진행.

## 6. 후행 단계에서 선행 결함 발견 시

1. 즉시 작업 중단 + HU에 보고.
2. 선행 단계 문서 개정 (드래프트).
3. 영향 받는 후행 산출물 재검토.
4. HU 재승인 후 진행 재개.

## 7. 사이즈 예산 추적 (모든 단계 공통)

- 모든 commit에서 자동 측정 (CI 또는 pre-commit hook으로 도입 — Gate 1에서 확정).
- 예산 위반 시 commit 차단 또는 경고.
- 위반 발생 시 `docs/02-design/size-budget.md` 갱신 + HU 보고.

## 8. 멀티 AI 호출 최소 횟수 (단계별 가이드)

| Gate | 최소 cross-review 라운드 |
|---|---|
| 0 | 1회 (CX + GE) |
| 1 | 1회 (CX + GE + GL) |
| 2 | 2회 (CX + GE + GL + OC) — 설계 + PoC |
| 3 | 주요 모듈마다 1회 (CX + OC) |
| 4 | 챕터마다 1회 (GE + GL) + 전체 게임플레이 1회 |
| 5 | 1회 (CX + OC) — 사이즈 close |
| 6 | 1회 (OC) — 빌드 검증 |

부족하면 추가 호출. **상한 없음**.

## 9. AI 도구 가용성 (현재 시점)

이 워크스페이스에서 직접 호출 가능한 AI:

- Claude (본 세션, 항상 가용)
- 기타 (Codex/Gemini/GLM/Opencode): pi substrate 또는 CLI 직접 호출 — **Gate 1 진입 전 가용성 점검 필요** (도구별 권한·API 키·rate limit 확인).

## 10. 본 프로세스 자체의 개정

- 운용 중 비효율 발견 시 즉시 개정안 작성 → HU 검토 → 통과 → 버전 증가.
- 게이트 우회·생략 같은 **구조적 변경**은 v2.0.
