# 검증 라운드 — 한글 자모 비트맵 슬라이스 1

**날짜:** 2026-05-30
**대상:** ㄱㄴㄷㄹㅁ 실제 8x16 비트맵 (placeholder 교체)

## 프로세스 (방법론: 생성계약 → 독립생성 → 검증 → 수렴)

| 단계 | 산출물 | 결과 |
|------|--------|------|
| 생성 계약 | `docs/contracts/jamo-bitmap-gen-2026-05-30.md` | 5 자모, 8x16, 실측 brightness, 합격기준 명세 |
| 독립 생성 | CX(codex) + GE(claude) | **2/2 완전 수렴** (계약 정밀명세 → 동일 정답) |
| 후보 | `docs/candidates/jamo-bitmap-2026-05-30.md` | 채택 비트맵 + 렌더 |
| 통합 | `tools/gen_jamo.py` (재현가능 생성기) | glyph_data.c 5 인덱스 교체 + brightness 68 재계산 |
| 검증 | `tools/verify_glyphs.py` + `make test` | 아래 |

## 검증 결과

1. **렌더 검증(시각)**: 5개 모두 실제 자모 모양 (ㄱ 상단바+우하강, ㄴ L, ㄷ ⊏, ㄹ 완전구조, ㅁ □). OC 육안 확인.
2. **🔴 회귀 발견·수정 (테스트가 잡음)**: 1차 통합서 `JAMO_BRIGHTNESS` 를 *raw popcount*(15,20,24,28)로 → 기존 테스트 `all values 0-15` **FAIL**(OOB). 계약의 "16단계(0-15)" 위반. → **16단계 스케일** `(popcount*15+64)//128` 로 수정(= cycle#4 claude 가 예측한 `count*15/128`). brightness = 2,2,2,3,3.
3. **합격 게이트**: `tools/verify_glyphs.py` → **VERDICT=PASS** (슬라이스 non-blank + brightness 68개 실측·범위 일치). placeholder 상태선 FAIL → 통합 후 PASS (red→green 실증).
4. **빌드+회귀**: `make` 27328 bytes (1.44MB 예산의 1.9%) + `make test` **17/17 PASS**.

## 방법론 관찰

- **정밀 명세 → 수렴↑/다양성↓**: 계약이 각 자모 모양을 서술하니 독립 생성이 동일 정답으로 수렴. 단순·잘정의 슬라이스엔 정상(correctness 신호). 다양성은 *under-specified* 창작에 가치.
- **테스트가 적대 리뷰를 보완**: 적대 리뷰(생성)는 모양을, 기계검증(기존 test + verify_glyphs)은 범위/실측을 잡음. 두 층 방어심층 실증 — brightness OOB 는 사람/리뷰어가 놓치기 쉬웠으나 기존 unit test 가 즉시 검출.

## 잔여 (다음 슬라이스)

- 나머지 자음 14 + 모음 21 + 겹자모 = 63 자모 실제 비트맵 (현 placeholder).
- ASCII 95 글리프 (현 blank).
- 슬라이스 1 = 프로세스·도구·게이트 확립. 이후는 같은 파이프라인 반복.
