#!/usr/bin/env python3
"""verify_glyphs — 글리프 합격 기준(기계검증). 계약 docs/contracts/jamo-bitmap-gen-2026-05-30.md.

검사:
 1. 슬라이스 1 자모(ㄱㄴㄷㄹㅁ, 인덱스 95/97/98/100/101) non-blank.
 2. JAMO_BRIGHTNESS[i] == popcount(FONT_GLYPHS[95+i]) 전 자모 (계약 "실측").
exit 0 = 합격. exit 1 = 위반(완료선언 차단용 — 강한 증거).
"""
import re, sys, pathlib

SRC = pathlib.Path(__file__).resolve().parent.parent / "src/engine/font/glyph_data.c"
ASCII_COUNT, JAMO_COUNT = 95, 68
SLICE1 = {95: "ㄱ", 97: "ㄴ", 98: "ㄷ", 100: "ㄹ", 101: "ㅁ"}

def popcount16(b): return sum(bin(x).count("1") for x in b)
def brightness16(b): return (popcount16(b) * 15 + 64) // 128  # 0-15 스케일(실측)

def main():
    t = SRC.read_text()
    body = re.search(r"FONT_GLYPHS\[GLYPH_TOTAL\]\s*=\s*\{(.*?)\n\};", t, re.S).group(1)
    glyphs = [[int(x, 16) for x in e.split(",") if x.strip()]
              for e in re.findall(r"\{\{([0-9a-fA-Fx,\s]+)\}\}", body)]
    jb_raw = re.search(r"JAMO_BRIGHTNESS\[GLYPH_JAMO_COUNT\]\s*=\s*\{([^}]+)\}", t).group(1)
    brightness = [int(x) for x in jb_raw.replace("\n", "").split(",") if x.strip()]

    fails = []
    # 1. 슬라이스 non-blank
    for idx, name in SLICE1.items():
        if popcount16(glyphs[idx]) == 0:
            fails.append(f"슬라이스 자모 {name}(idx {idx}) blank")
    # 2. brightness == popcount (전 자모)
    mism = 0
    for i in range(JAMO_COUNT):
        if brightness[i] != brightness16(glyphs[ASCII_COUNT + i]) or not (0 <= brightness[i] <= 15):
            mism += 1
    if mism:
        fails.append(f"JAMO_BRIGHTNESS 실측 위반 {mism}/{JAMO_COUNT}개 (popcount 불일치)")

    if fails:
        print("VERDICT=FAIL")
        for f in fails: print(f"  - {f}")
        sys.exit(1)
    print(f"VERDICT=PASS — 슬라이스1 자모 {len(SLICE1)}개 non-blank + JAMO_BRIGHTNESS {JAMO_COUNT}개 실측 일치.")
    sys.exit(0)

if __name__ == "__main__":
    main()
