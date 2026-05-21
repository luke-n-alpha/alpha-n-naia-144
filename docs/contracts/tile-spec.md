# Tile Spec — alpha-n-naia-144

**상태:** Draft v0.2 (CX+GE 리뷰 반영)
**변경:** glyph/fb 포맷 분리, 밝기 매핑 실측 자동화, pixel mode 재정의

---

## Glyph Asset Format

| 항목 | 값 |
|------|-----|
| 글리프 크기 | 8 × 16 픽셀 |
| 비트 깊이 | 1-bit (흑백) |
| 인코딩 | UTF-8 |
| 저장 | 16 bytes per glyph |

## Framebuffer Format

| 모드 | 픽셀 포맷 | 비고 |
|------|----------|------|
| Text | (char_code, fg_4bit, bg_4bit) per cell | 셀 단위 |
| Pixel | 4-bit color index per pixel | 픽셀 단위 |

---

## 글리프 집합

### 한글 자모 (68개)

초성 19 + 중성 21 + 종성 28 = 68 자모
68 × 16 bytes = 1,088 bytes

### ASCII (95개)

0x20-0x7E, 95 × 16 bytes = 1,520 bytes

### 박스 드로잉 + UI 기호 (~40개)

┌ ┐ └ ┘ │ ─ ├ ┤ ┬ ┴ ┼ ▀ ▄ ▌ ▐ █ ▓ ▒ ░ 등
~40 × 16 bytes = 640 bytes

### 총 글리프

~203 글리프 × 16 bytes ≈ 3.2 KiB (패딩/정렬 포함 4 KiB)

---

## 밝기 매핑

### 원칙

각 자모 비트맵의 **on-pixel count**를 실측하여 자동 정렬. 수동 테이블 불사용.

### 생성 방식

```bash
# 자모 밝기 자동 측정 스크립트 (Phase A-2에서 구현)
python3 tools/measure_brightness.py assets/fonts/jamo.bdf > src/jamo_brightness.c
```

출력: `const uint8_t JAMO_BRIGHTNESS[68] = {...};` (0=밝음, 15=어두움)

### 밝기 레벨

16단계 (0-15). 8단계로 축소 가능 (실측 후 결정).

---

## 듀얼 렌더 모드

### Text Mode

- 화면을 셀 그리드로 분할 (예: 80×25)
- 각 셀: 글리프 + fg색 + bg색
- 대화, 메뉴, 탐색에 사용

### Pixel Mode

- 동일 셀 그리드 위에서 **픽셀 단위** 제어
- 가상 해상도: cols×8 × rows×16 (예: 80×8=640 × 25×16=400)
- 전투, 이펙트에 사용
- 타일(8×16) 단위 블릿 연산 지원

---

## 시각 모티프 (cafelua 세계관)

콘텐츠 기획 시 반드시 포함할 시각 요소:

| 모티프 | 소스 | 타일 표현 방식 |
|--------|------|---------------|
| 카페 외관 (사계절) | cafelua_space_concept.md | 밝기 그라데이션 + 색 팔레트 |
| 카페 내부 (카운터, 피아노, 소파) | cafelua_space_concept.md | 박스 드로잉 + 자모 조합 |
| 루크 | luke_profile.md | 텍스트 + emotion_id |
| Alpha (메이드) | alpha-appearance-description.ko.md | 텍스트 + emotion_id |
| naia (고양이) | naia page.tsx | 자모 밝기 패턴 |
| 검은 고양이 | cafelua_space_concept.md | 어두운 자모 타일 |
| 2층 아틀리에 | cafelua_space_concept.md | 박스 드로잉 |

---

## 열린 질문 (잔여)

1. Alpha compositing (중성+종성 겹침)을 전투 이펙트에만 제한 적용할지?
2. 박스 드로잉 문자를 40개로 충분한가, 더 필요한가?
