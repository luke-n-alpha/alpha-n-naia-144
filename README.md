# alpha-n-naia-144

> 한글 자모를 그래픽 타일로 사용하는 1.44MB 디스켓 게임 + 그것을 위해 자체 구현한 터미널/폰트 렌더 엔진.

**상태:** Charter 단계 (SDLC Gate 0) — 요구사항 정의 전.
**소속:** `luke_n_alpha/alpha-n-naia-144` (개인 프로젝트).
**라이선스:** TBD (오픈소스 공개 예정, 구체 라이선스는 Charter 검토 이후 결정).

---

## 한 줄 컨셉

> **"한글 자모로 그려진, 자기 자신을 그리는 터미널 게임. 디스켓 한 장에."**

## 두 가지 동시 목표

1. **출품**: 2PGameArcade *1.44MB 게임 공모전* (마감 2026-09-04 23:39).
2. **공개**: "AI(Claude/Codex/Gemini/GLM/Opencode 등 멀티 도구)로 어떻게 게임을 개발하는가"의 살아있는 사례를 오픈소스로 공개.

게임은 1.44MB 한 장에 자급자족. AI 개발 메타는 commit 히스토리 · 산출물 문서 · 세션 로그로 외부에 드러난다.

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
│   ├── context/                    # 규칙·인덱스 (rules + project-index)
│   ├── progress/                   # 단계 진행 보고서
│   └── work/                       # 임시 작업 (gitignored)
├── .users/ko/context/              # 사람용 한국어 mirror
├── docs/                           # SDLC 산출물
│   ├── 00-charter/                 # 헌장 · AI 역할 · 프로세스
│   ├── 01-requirements/            # 요구사항
│   ├── 02-design/                  # 설계
│   ├── 03-implementation/          # 구현 노트
│   ├── 04-testing/                 # 테스트 계획·결과
│   ├── 05-deployment/              # 배포 (디스켓 이미지 · 공모전 제출)
│   └── 06-maintenance/             # 유지보수 · 오픈소스 운영
├── src/                            # 소스코드
├── assets/{fonts,scenarios,sfx}/   # 폰트·시나리오·사운드
├── tests/                          # 테스트
├── tools/                          # 빌드/사이즈/이미지 도구
└── build/                          # 빌드 산출물 (gitignored)
```

## SDLC

이 프로젝트는 규모는 작지만 **SDLC를 철저히 준수**한다. 각 단계 산출물은 `docs/0N-*` 하위에 두고, 사용자(luke) 검토 게이트를 통과해야 다음 단계 진행. 상세는 [`docs/00-charter/sdlc-process.md`](docs/00-charter/sdlc-process.md).

## AI 협업

본 프로젝트는 단일 AI가 아닌 **멀티 도구 앙상블**로 진행한다. 역할 분담과 RACI는 [`docs/00-charter/ai-roles.md`](docs/00-charter/ai-roles.md).

## 진행 상태

- [x] **Gate 0 — Charter**: 헌장 · AI 역할 · SDLC 프로세스 정의 (현재 단계, 사용자 검토 대기)
- [ ] **Gate 1 — Requirements**
- [ ] **Gate 2 — Design**
- [ ] **Gate 3 — Implementation (engine core)**
- [ ] **Gate 4 — Implementation (game content)**
- [ ] **Gate 5 — Testing & Size budget close**
- [ ] **Gate 6 — Deployment (disk image + 공모전 제출)**
- [ ] **Gate 7 — Post-release (오픈소스 공개)**
