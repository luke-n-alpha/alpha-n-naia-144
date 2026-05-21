# Gate 2 — Design

**상태:** Pending (Gate 1 통과 후 시작)
**기간:** 2026-06-01 ~ 2026-06-14 (2주)

## 산출물 계획

| 파일 | 내용 |
|---|---|
| `architecture.md` | 4계층 (Game Logic / ANSI VM / Font Renderer / Window) |
| `jamo-mapping.md` | 한글 자모 픽셀↔글자 매핑 알고리즘 (shape matching, lookup table) |
| `terminal-engine.md` | 자체 터미널 명세 (libtsm fork or 직접 구현, VT 시퀀스 subset) |
| `font-renderer.md` | 비트맵 폰트 렌더러 (갈무리/자체 자모 비트맵, 더블 버퍼) |
| `dual-render.md` | 글자/픽셀 듀얼 렌더 파이프라인 (mode switch, 블록 매핑) |
| `size-budget.md` | 1.44MB 항목별 예산 (엔진/폰트/시나리오/사운드/룰베이스 AI 분배) |
| `game-design.md` | 게임 디자인 (서사·NPC·룰·승리조건·세션 길이) |
| `characters.md` | cafelua 카페 / naia 고양이 + 조연 |
| `poc-jamo-frames.md` | PoC — 정지 frame 5장 자모 매핑 시각 검증 |

## 시작 체크리스트

- [ ] Gate 1 통과 신호 수신 (HU)
- [ ] 본 README 갱신 (in_progress)
- [ ] CL 초안 작성
- [ ] CX + GE + GL + OC cross-review
- [ ] PoC 실행 (정지 frame 5장 → 자모 매핑 결과 시각 확인)
- [ ] 통합 → HU 검토 게이트
