#!/usr/bin/env python3
"""gen_jamo — 실제 한글 자모 비트맵을 glyph_data.c 에 통합 (재현 가능 생성기).

- 슬라이스 1 자모(ㄱㄴㄷㄹㅁ) 실제 8x16 비트맵을 FONT_GLYPHS 해당 인덱스에 기입.
- JAMO_BRIGHTNESS 를 *모든* 자모의 실제 popcount 로 재계산(계약 "on-pixel 실측" — 가짜 시퀀스 폐기).
하드코딩 hex 를 손편집하지 않고 이 생성기가 결정론적으로 패치한다. (docs/candidates/jamo-bitmap-2026-05-30.md 채택분)
"""
import re, sys, pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent
SRC = ROOT / "src/engine/font/glyph_data.c"

ASCII_COUNT = 95
# 채택 자모: FONT_GLYPHS 인덱스(=95+jamo_idx) → 16바이트
JAMO = {
    95:  [0x00,0x00,0x00,0x7E,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x00,0x00,0x00],  # ㄱ
    97:  [0x00,0x00,0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7E,0x00,0x00,0x00],  # ㄴ
    98:  [0x00,0x00,0x00,0x7E,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7E,0x00,0x00,0x00],  # ㄷ
    100: [0x00,0x00,0x00,0x7E,0x02,0x02,0x02,0x7E,0x40,0x40,0x40,0x7E,0x00,0x00,0x00,0x00],  # ㄹ
    101: [0x00,0x00,0x00,0x7E,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x7E,0x00,0x00,0x00],  # ㅁ
}

def popcount16(b): return sum(bin(x).count("1") for x in b)
# 계약: 밝기 16단계(0-15) = on-pixel count(0..128) 를 스케일(실측). raw popcount 아님(cycle#4 claude 지적).
def brightness16(b): return (popcount16(b) * 15 + 64) // 128

def main():
    text = SRC.read_text()
    # FONT_GLYPHS 엔트리 추출 {{...}}
    m = re.search(r"(FONT_GLYPHS\[GLYPH_TOTAL\]\s*=\s*\{)(.*?)(\n\};)", text, re.S)
    head, body, tail = m.group(1), m.group(2), m.group(3)
    entries = re.findall(r"\{\{([0-9a-fA-Fx,\s]+)\}\}", body)
    glyphs = [[int(x, 16) for x in e.split(",") if x.strip()] for e in entries]
    assert len(glyphs) == 203, f"glyph 수 {len(glyphs)} != 203"
    # 자모 교체
    for idx, data in JAMO.items():
        glyphs[idx] = data
    new_body = "\n" + ",\n".join("    {{" + ",".join(f"0x{b:02X}" for b in g) + "}}" for g in glyphs) + "\n"
    text = text[:m.start()] + head + new_body + tail + text[m.end():]
    # JAMO_BRIGHTNESS = 모든 자모 실제 popcount (인덱스 95..95+68)
    jb = [brightness16(glyphs[ASCII_COUNT + i]) for i in range(68)]
    text = re.sub(
        r"(JAMO_BRIGHTNESS\[GLYPH_JAMO_COUNT\]\s*=\s*\{)[^}]*\}",
        lambda _: "JAMO_BRIGHTNESS[GLYPH_JAMO_COUNT] = {\n    " + ",".join(str(v) for v in jb) + "\n}",
        text,
    )
    SRC.write_text(text)
    print(f"[gen_jamo] 자모 {len(JAMO)}개 통합 + JAMO_BRIGHTNESS 68개 실측 재계산.")
    print(f"  채택 popcount: ㄱ={popcount16(JAMO[95])} ㄴ={popcount16(JAMO[97])} ㄷ={popcount16(JAMO[98])} ㄹ={popcount16(JAMO[100])} ㅁ={popcount16(JAMO[101])}")

if __name__ == "__main__":
    main()
