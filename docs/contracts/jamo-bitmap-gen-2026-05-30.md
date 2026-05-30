# 생성 계약 — 한글 자모 비트맵 (Phase A-2, 슬라이스 1)

**작성:** OC (오케스트레이터)
**날짜:** 2026-05-30
**상태:** 독립 생성 디스패치 대상

## 배경

`src/engine/font/glyph_data.c` 의 자모 글리프가 placeholder(기하 패턴) + `JAMO_BRIGHTNESS` 가
가짜(0,1,..15 반복). 로직(`font.c font_get_glyph`)·빌드는 정상. **실제 한글 자모 비트맵**으로 교체한다.

## 슬라이스 범위 (바운디드 — 검증 가능 우선)

기본 자음 **5개**부터: ㄱ(0x3131) ㄴ(0x3134) ㄷ(0x3137) ㄹ(0x3139) ㅁ(0x3141).
※ FONT_GLYPHS 인덱스 = GLYPH_ASCII_COUNT(95) + JAMO_CODEPOINTS 내 위치. (ㄱ=95+0, ㄴ=95+2, ㄷ=95+3, ㄹ=95+5, ㅁ=95+6 — `font.c` JAMO_CODEPOINTS 순서 기준.)

## 하드 제약 (계약)

| 항목 | 값 |
|------|-----|
| 크기 | 8 × 16 px, 1-bit. `GlyphBitmap.pixels[16]`, 각 바이트 = 한 행(MSB=좌측 픽셀, `0x80>>col`). |
| 형태 | **실제 한글 자모 모양**이어야 함 (기하 패턴 금지). 8px 폭에 자모 1개, 세로 중앙 정렬. |
| 여백 | 좌/우 최소 1px, 상/하 약간 — 글자가 셀에 꽉 차지 않게. |
| 밝기 | `JAMO_BRIGHTNESS[i]` = **실제 popcount(glyph[i])** (실측, 가짜 시퀀스 금지). |

## 합격 기준 (기계검증 — `tools/verify_glyphs.py`)

1. 대상 5개 글리프 **non-blank** (popcount > 0).
2. `JAMO_BRIGHTNESS[i] == popcount(FONT_GLYPHS[idx_i])` for 대상 i.
3. 빌드 통과 (`make`) + 기존 17 테스트 회귀 0.
4. **렌더 검증**: 각 글리프를 8x16 ASCII-art 로 출력 → 사람/리뷰어가 자모 모양 확인 (시각 = 적대 리뷰 몫).

## 프로세스

1. **독립 생성** (CX=codex, GE=claude): 각자 5개 자모의 `pixels[16]` hex 16바이트 ×5 를 생성. 컨텍스트 격리(상대 결과 미노출 = 안티앵커링).
2. **OC 병합·렌더**: 후보를 ASCII-art 렌더 → 더 정확한 쪽 채택/병합.
3. **다중 검증**: codex+claude 가 렌더 결과를 적대 리뷰 (모양이 실제 ㄱㄴㄷㄹㅁ 인가).
4. **수렴**: glyph_data.c 대상 인덱스 교체 + brightness 실측 + verify_glyphs green → commit (강한 증거 인용).

## 산출물

- 이 계약 (`docs/contracts/jamo-bitmap-gen-2026-05-30.md`)
- 후보 (`docs/candidates/jamo-cx.md`, `jamo-ge.md`)
- 검증 라운드 (`docs/rounds/jamo-bitmap-review-2026-05-30.md`)
- 합격 도구 (`tools/verify_glyphs.py`)
- 수렴 결과 (glyph_data.c diff + commit)
