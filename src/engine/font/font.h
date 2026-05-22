#ifndef FONT_H
#define FONT_H

#include <stdint.h>

#define FONT_WIDTH  8
#define FONT_HEIGHT 16
#define FONT_BPP    1
#define GLYPH_BYTES (FONT_WIDTH * FONT_HEIGHT / 8)

typedef struct {
    uint8_t pixels[GLYPH_BYTES];
} GlyphBitmap;

typedef struct {
    uint16_t codepoint;
    uint8_t  index;
} GlyphEntry;

#define GLYPH_ASCII_START  0x20
#define GLYPH_ASCII_COUNT  95
#define GLYPH_JAMO_COUNT   68
#define GLYPH_BOX_COUNT    40
#define GLYPH_TOTAL        (GLYPH_ASCII_COUNT + GLYPH_JAMO_COUNT + GLYPH_BOX_COUNT)

extern const GlyphBitmap FONT_GLYPHS[GLYPH_TOTAL];
extern const uint8_t JAMO_BRIGHTNESS[GLYPH_JAMO_COUNT];

const GlyphBitmap *font_get_glyph(uint32_t codepoint);

#endif
