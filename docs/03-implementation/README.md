# Gate 3-4 — Implementation

**상태:** Pending
**기간:** 2026-06-15 ~ 2026-08-09 (8주, Engine 4주 + Game 4주)

## 산출물 계획

### Gate 3 — Engine Core (6/15 ~ 7/12)

| 파일 | 내용 |
|---|---|
| `engine.md` | 엔진 구현 노트 (모듈 구조, 빌드 옵션, 사이즈 측정) |
| `src/engine/` | 터미널 + 폰트 + 듀얼 렌더 + 입력 |
| `tests/engine/` | 단위 테스트 |

### Gate 4 — Game Content (7/13 ~ 8/9)

| 파일 | 내용 |
|---|---|
| `game.md` | 게임 구현 노트 (시나리오 엔진, NPC 룰 AI, 챕터 구조) |
| `src/game/` | 게임 본체 |
| `assets/scenarios/` | 시나리오 텍스트 |
| `assets/fonts/` | 비트맵 폰트 |
| `assets/sfx/` | 사운드 |

## 시작 체크리스트 (Gate 별)

- [ ] 직전 Gate 통과 신호 수신 (HU)
- [ ] 본 README 갱신 (in_progress)
- [ ] 구현 + 매 commit 사이즈 측정
- [ ] 주요 모듈마다 CX + OC cross-review
- [ ] 챕터마다 GE + GL cross-review (Gate 4)
- [ ] HU 검토 게이트
