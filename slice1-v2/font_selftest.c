#include "font.h"

#include <stdio.h>
#include <string.h>

static int verify_lookup(void) {
    const Glyph8x16 latin_a = font_get_glyph(FONT_GLYPH_LATIN_A);
    const Glyph8x16 ieung = font_get_glyph(FONT_GLYPH_IEUNG);
    const Glyph8x16 missing = font_get_glyph(-1);
    const Glyph8x16 notdef = font_get_glyph(FONT_GLYPH_NOTDEF);

    if (latin_a.rows[0] != 0x38 || latin_a.rows[6] != 0x7C) {
        fprintf(stderr, "lookup failed: LATIN_A rows mismatch\n");
        return 1;
    }

    if (ieung.rows[0] != 0x38 || ieung.rows[10] != 0x38) {
        fprintf(stderr, "lookup failed: IEUNG rows mismatch\n");
        return 1;
    }

    if (memcmp(&missing, &notdef, sizeof(Glyph8x16)) != 0) {
        fprintf(stderr, "lookup failed: invalid id did not map to .notdef\n");
        return 1;
    }

    return 0;
}

static int verify_draw(void) {
    RenderCore core;
    const int glyphs[] = {
        FONT_GLYPH_LATIN_A,
        FONT_GLYPH_DIGIT_1,
        FONT_GLYPH_SPACE,
        FONT_GLYPH_GIYEOK
    };
    const Glyph8x16 expected_first = font_get_glyph(FONT_GLYPH_LATIN_A);
    const Glyph8x16 expected_third = font_get_glyph(FONT_GLYPH_SPACE);
    const Glyph8x16 expected_space = font_get_glyph(FONT_GLYPH_SPACE);
    const Cell *cell;

    core_init(&core);
    font_draw_glyphs(&core, 77, 3, glyphs, (int)(sizeof(glyphs) / sizeof(glyphs[0])), 9u, 14u, 1u);

    cell = &core.cells[3][77];
    if (memcmp(&cell->glyph, &expected_first, sizeof(Glyph8x16)) != 0 ||
        cell->brightness != 9u || cell->fg_index != 14u || cell->bg_index != 1u) {
        fprintf(stderr, "draw failed: cell 77,3 mismatch\n");
        return 1;
    }

    cell = &core.cells[3][79];
    if (memcmp(&cell->glyph, &expected_third, sizeof(Glyph8x16)) != 0) {
        fprintf(stderr, "draw failed: cell 79,3 mismatch\n");
        return 1;
    }

    if (memcmp(&core.cells[3][0].glyph, &expected_space, sizeof(Glyph8x16)) != 0 ||
        core.cells[3][0].brightness != 0u) {
        fprintf(stderr, "draw failed: out-of-range write leaked into untouched cell\n");
        return 1;
    }

    return 0;
}

int main(void) {
    if (verify_lookup() != 0) {
        return 1;
    }

    if (verify_draw() != 0) {
        return 1;
    }

    if (font_selftest(1) != 0) {
        return 1;
    }

    puts("font_selftest=PASS");
    return 0;
}
