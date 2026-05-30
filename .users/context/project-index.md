<!-- src-sha: 4d5c0b3a0251a023 -->
<!-- 자동 번역 미러 (M13-mirror). 원본: .agents/context/project-index.yaml -->

# alpha-n-naia-144 프로젝트 문서 구조

**프로젝트:** 1.44MB 디스켓 게임 (한글 자모 기반)  
**문서 버전:** 초안 v0.1  
**작성일:** 2026-05-21

---

## 진입점

| 대상 | 파일 |
|------|------|
| 일반 사용자 | `README.md` |
| 모든 도구(Claude·Gemini·OpenCode) | `AGENTS.md`, `CLAUDE.md`, `GEMINI.md` |
| AI 공식 정보원 (SoT) | `.agents/context/agents-rules.json` |

---

## 세션 시작 시 필독 문서

1. **`docs/00-charter/charter.md`**  
   목표, 범위, 제약, 일정, 위험, 결재 권한

2. **`docs/00-charter/ai-roles.md`**  
   AI 역할 분담 및 책임 매트릭스 (RACI: 책임자·담당자·상의·통보)

3. **`docs/00-charter/sdlc-process.md`**  
   소프트웨어 개발 생명주기 (SDLC) 게이트 정의 및 검토 절차

---

## 개발 단계별 산출물

### 단계 0: 기본 계획 (진행 중)

**폴더:** `docs/00-charter/`

| 파일 | 내용 |
|------|------|
| `charter.md` | 프로젝트 헌장 |
| `ai-roles.md` | AI 역할 및 책임 |
| `sdlc-process.md` | 개발 생명주기 프로세스 |

**상태:** 진행 중

---

### 단계 1: 요구사항 (예정)

**폴더:** `docs/01-requirements/`

| 파일 | 내용 |
|------|------|
| `functional.md` | 기능 요구사항 |
| `non-functional.md` | 비기능 요구사항 (용량, 성능, 호환성) |
| `user-scenarios.md` | 사용자 시나리오 및 게임 흐름 |
| `contest-rules.md` | 공모전 규정 재확인 |
| `license.md` | 라이선스 결정 |

**상태:** 예정

---

### 단계 2: 설계 (예정)

**폴더:** `docs/02-design/`

| 파일 | 내용 |
|------|------|
| `architecture.md` | 4계층 아키텍처 |
| `jamo-mapping.md` | 한글 자모를 픽셀에 매핑하는 방식 |
| `terminal-engine.md` | 자체 터미널 엔진 명세 |
| `font-renderer.md` | 비트맵 폰트 렌더러 명세 |
| `dual-render.md` | 글자/픽셀 이중 렌더링 모드 |
| `size-budget.md` | 1.44MB 용량 항목별 예산 |
| `game-design.md` | 게임 디자인 (이야기, NPC, 규칙) |
| `characters.md` | cafelua·naia 고양이 캐릭터 |
| `poc-jamo-frames.md` | 자모 매핑 개념 검증 (정지 이미지 5장) |

**상태:** 예정

---

### 단계 3: 구현 (예정)

**폴더:** `docs/03-implementation/`  
**코드 폴더:** `src/engine/`, `src/game/`  
**자산 폴더:** `assets/scenarios/`

| 파일 | 내용 |
|------|------|
| `engine.md` | 엔진 구현 기록 |
| `game.md` | 게임 콘텐츠 구현 기록 |

**상태:** 예정

---

### 단계 4: 테스트 (예정)

**폴더:** `docs/04-testing/`  
**코드 폴더:** `tests/`

| 파일 | 내용 |
|------|------|
| `test-plan.md` | 테스트 계획 |
| `results.md` | 테스트 결과 |
| `size-final.md` | 최종 용량 확인 |

**상태:** 예정

---

### 단계 5: 배포 (예정)

**폴더:** `docs/05-deployment/`  
**산출물:** `build/disk.img`

| 파일 | 내용 |
|------|------|
| `disk-image.md` | 디스켓 이미지 생성 방법 |
| `submission.md` | 공모전 제출 절차 |
| `readme-bilingual.md` | 한영 이중 언어 README |

**상태:** 예정

---

### 단계 6: 출시 후 (예정)

**폴더:** `docs/06-maintenance/`

| 파일 | 내용 |
|------|------|
| `open-source-launch.md` | 오픈소스 공개 계획 |
| `retrospective.md` | 사후 회고 |

**상태:** 예정

---

## AI 활동 기록 폴더

| 용도 | 폴더 | 설명 |
|------|------|------|
| 현재 진행 중 | `.agents/progress/` | 활동 기록 (마크다운) |
| 완료·보관 | `.agents/progress/archive/` | 월별(`YYYY-MM/`) 보관 |
| 검토 결과 | `.agents/progress/reviews/` | 원본 검토 의견 |
| 의사결정 | `.agents/progress/decisions-YYYY-MM-DD.md` | 날짜별 결정 이력 |
| 임시 파일 | `.agents/work/` | gitignore 대상 |

---

## 사람용 미러

**폴더:** `.users/ko/context/`

- **동기화 방식:** 수동
- **원본(SoT):** `docs/` 폴더
- **용도:** `.users/ko/context/`는 보조 미러 역할

본 파일(`AGENTS.md`, `CLAUDE.md`, `GEMINI.md`) 3개는 항상 동일 내용을 유지합니다.
