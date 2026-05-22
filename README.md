# alpha-n-naia-144

> 한글 자모를 그래픽 타일로 사용하는 1.44MB 디스켓 게임 + 그것을 위해 자체 구현한 터미널/폰트 렌더 엔진.

**상태:** 방법론 응용 실험 진행 중 — 생성 계약 v0.2 확정, 첫 모듈 생성 대기.
**소속:** `luke_n_alpha/alpha-n-naia-144` (개인 프로젝트).
**라이선스:** TBD (오픈소스 공개 예정).

> **2026-05-22 전환**: 기존 SDLC 게이트 방식에서 **특허 10-2026-0056403(멀티에이전트 상호 검증 프레임워크)의 방법론을 게임 개발 전체에 응용하는 자율 실험**로 전환. 사용자(luke)는 방향을 제공하지 않고, 다수 AI 에이전트가 생성 계약 기반으로 자율 개발. 상세는 [`docs/00-charter/methodology-research.md`](docs/00-charter/methodology-research.md).

---

## 한 줄 컨셉

> **"한글 자모로 그려진, 자기 자신을 그리는 터미널 게임. 디스켓 한 장에."**

## 목적

이 프로젝트는 **두 가지 산출물**을 동시에 목표로 한다:

1. **하네스 아키텍처** (진짜 목적): AI 에이전트들이 SDLC에 따라 체계적으로 소프트웨어를 생산하는 재사용 가능한 하네스. 특허 10-2026-0056403 방법론을 실증하고, 결과를 **naia-adk에 이식**.
2. **1.44MB 디스켓 게임** (실험 매개체): 하네스를 검증하기 위한 실제 프로젝트. 한글 자모 타일 게임을 공모전(2PGameArcade, 마감 2026-09-04)에 출품.

게임은 하네스가 작동하는지 증명하는 수단. 과정은 commit 히스토리 · 산출물 문서 · 회고 보고서로 완전 공개.

## 차별점

- **한글 자모 = 비트맵 타일셋**: 영숫자 brightness ramp(`@%#*+=-:.`)가 아니라 자모(ㅇ ㅁ ㅅ ㄱ ㄴ ㄹ ㅎ ㅋ …)의 **기하학적 형태**를 그대로 픽셀↔글자 매핑에 사용.
- **자체 터미널 + 자체 폰트 렌더러**: 호스트 터미널/폰트 비의존. "터미널에서 도는 게임"이 아니라 **"자기 자신이 터미널인 게임"**.
- **글자 모드 ↔ 픽셀 모드 듀얼 렌더**: 같은 바이너리에서 텍스트 어드벤처(글자 단위)와 픽셀 액션(글자=8×16 캔버스)을 전환.
- **소재**: `cafelua` 한글 카페 무대 + `naia` 고양이 캐릭터.

## 폴더 구조

```
alpha-n-naia-144/
├── README.md                       # 본 문서
├── AGENTS.md = CLAUDE.md = GEMINI.md   # 멀티툴 entry (동일 내용)
├── .agents/                        # AI SoT
│   ├── context/                    # 규칙·인덱스
│   ├── progress/                   # 진행 보고서
│   └── work/                       # 임시 작업 (gitignored)
├── .users/ko/context/              # 사람용 한국어 mirror
├── docs/
│   ├── 00-charter/                 # 방법론, 조직, 프로세스, 세션 로그
│   ├── contracts/                  # 생성 계약 (tech-stack, size-budget, interfaces, tile-spec)
│   ├── rounds/                     # 라운드별 검증 기록
│   └── 01-06/                      # (legacy SDLC placeholder — 미사용)
├── src/                            # 소스코드
├── assets/{fonts,scenarios,sfx}/   # 폰트·시나리오·사운드
├── tests/                          # 테스트
├── tools/                          # 빌드/사이즈/이미지 도구
└── build/                          # 빌드 산출물 (gitignored)
```

## 방법론

특허 10-2026-0056403(멀티에이전트 상호 검증 프레임워크)의 방법론을 게임 개발 전체에 응용.
독립 생성 → 제약 검사 → 후보 병합 → 다중 검증 → 수렴. 생성 계약 기반 좁은 모듈 단위 진행.
상세: [`methodology-research.md`](docs/00-charter/methodology-research.md) → [`agent-organization.md`](docs/00-charter/agent-organization.md) → [`process-and-deliverables.md`](docs/00-charter/process-and-deliverables.md).

## AI 에이전트

| ID | 모델 | 역할 |
|----|------|------|
| OC | GLM/Opencode | 오케스트레이터 + 중재 |
| CX | Codex | 생성자 + 검증자 (시스템/엔진) |
| GE | Gemini | 생성자 + 검증자 (시각/UX) |

상세: [`agent-organization.md`](docs/00-charter/agent-organization.md).

## 진행 상태

- [x] **Phase 0 — 방법론 설계**: 갭 분석 + 3-AI 교차 리뷰 + 조직/프로세스 설계
- [x] **Phase A-1 — 생성 계약**: tech-stack, size-budget, interfaces, tile-spec v0.2 확정
- [ ] **Phase A-2 — 게임 디자인**: 기획 생성 계약 (CX+GE 독립 생성)
- [ ] **Phase A-3 — 타일셋 + 밝기 매핑** (GE → CX 검증)
- [ ] **Phase A-4 — 폰트 렌더러** (CX → GE 검증)

→ **회고 #1** (Phase A 완료 후)

- [ ] **Phase B — 엔진 코어**: 렌더 백엔드, 게임 루프, 입력

→ **회고 #2**

- [ ] **Phase C — 게임 콘텐츠**: 시나리오, 챕터, NPC

→ **회고 #3**

- [ ] **Phase D — 통합·제출**: 디스켓 이미지, 공모전 제출 (2026-09-04)

→ **회고 #4** (최종 — 하네스 일반화 + naia-adk 이식 계획)
