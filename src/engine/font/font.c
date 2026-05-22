#include "font.h"
#include <string.h>

static int cp_to_index(uint32_t cp) {
    if (cp >= GLYPH_ASCII_START && cp < GLYPH_ASCII_START + GLYPH_ASCII_COUNT)
        return cp - GLYPH_ASCII_START;
    return -1;
}

const GlyphBitmap *font_get_glyph(uint32_t codepoint) {
    int idx = cp_to_index(codepoint);
    if (idx >= 0) return &FONT_GLYPHS[idx];
    return &FONT_GLYPHS[0];
}
