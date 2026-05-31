#include "core.h"

#include <string.h>

static const Glyph8x16 k_demo_giyeok = {
    .rows = {
        0xF8, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x08,
        0x08, 0x08, 0x08, 0x00,
        0x00, 0x00, 0x00, 0x00
    }
};

static uint32_t fnv1a_u32(const void *data, size_t size) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t hash = 2166136261u;
    size_t i;

    for (i = 0; i < size; ++i) {
        hash ^= bytes[i];
        hash *= 16777619u;
    }

    return hash;
}

static uint32_t glyph_on_pixel_count(const Glyph8x16 *glyph) {
    uint32_t count = 0;
    int y;
    int bit;

    for (y = 0; y < SLICE1_CELL_HEIGHT; ++y) {
        for (bit = 0; bit < SLICE1_CELL_WIDTH; ++bit) {
            if (glyph->rows[y] & (uint8_t)(0x80u >> bit)) {
                ++count;
            }
        }
    }

    return count;
}

void core_init(RenderCore *core) {
    static const uint32_t default_palette[16] = {
        0xFF000000u, 0xFF1D2B53u, 0xFF7E2553u, 0xFF008751u,
        0xFFAB5236u, 0xFF5F574Fu, 0xFFC2C3C7u, 0xFFFFF1E8u,
        0xFFFF004Du, 0xFFFFA300u, 0xFFFFEC27u, 0xFF00E436u,
        0xFF29ADFFu, 0xFF83769Cu, 0xFFFF77A8u, 0xFFFFCCAAu
    };

    memset(core, 0, sizeof(*core));
    memcpy(core->palette, default_palette, sizeof(default_palette));
    core_clear_cells(core, 0);
}

void core_clear_cells(RenderCore *core, uint8_t bg_index) {
    int y;
    int x;

    for (y = 0; y < SLICE1_GRID_HEIGHT; ++y) {
        for (x = 0; x < SLICE1_GRID_WIDTH; ++x) {
            Cell *cell = &core->cells[y][x];
            memset(cell, 0, sizeof(*cell));
            cell->bg_index = (uint8_t)(bg_index & 0x0Fu);
        }
    }
}

void core_set_cell(RenderCore *core, int x, int y, const Cell *cell) {
    if (x < 0 || x >= SLICE1_GRID_WIDTH || y < 0 || y >= SLICE1_GRID_HEIGHT) {
        return;
    }

    core->cells[y][x] = *cell;
}

void core_rasterize(RenderCore *core, uint32_t *out_on_pixels) {
    uint32_t on_pixels = 0;
    int cell_y;
    int cell_x;

    for (cell_y = 0; cell_y < SLICE1_GRID_HEIGHT; ++cell_y) {
        for (cell_x = 0; cell_x < SLICE1_GRID_WIDTH; ++cell_x) {
            const Cell *cell = &core->cells[cell_y][cell_x];
            const uint32_t on_color = core->palette[cell->brightness & 0x0Fu];
            const uint32_t off_color = core->palette[cell->bg_index & 0x0Fu];
            int py;
            int px;

            for (py = 0; py < SLICE1_CELL_HEIGHT; ++py) {
                const uint8_t row_mask = cell->glyph.rows[py];
                const int fb_y = cell_y * SLICE1_CELL_HEIGHT + py;

                for (px = 0; px < SLICE1_CELL_WIDTH; ++px) {
                    const int fb_x = cell_x * SLICE1_CELL_WIDTH + px;
                    const int bit_set = (row_mask & (uint8_t)(0x80u >> px)) != 0;

                    core->pixels[fb_y * SLICE1_FB_WIDTH + fb_x] = bit_set ? on_color : off_color;
                    if (bit_set) {
                        ++on_pixels;
                    }
                }
            }
        }
    }

    if (out_on_pixels != NULL) {
        *out_on_pixels = on_pixels;
    }
}

void core_build_demo(RenderCore *core, HeadlessReport *report) {
    Cell demo;
    uint32_t raster_on_pixels = 0;

    memset(report, 0, sizeof(*report));
    core_clear_cells(core, 0);

    memset(&demo, 0, sizeof(demo));
    demo.glyph = k_demo_giyeok;
    demo.brightness = 9;
    demo.fg_index = 14;
    demo.bg_index = 1;

    report->demo_cell.x = 12;
    report->demo_cell.y = 7;
    report->expected_brightness = demo.brightness;
    report->glyph_on_pixels = glyph_on_pixel_count(&demo.glyph);

    core_set_cell(core, report->demo_cell.x, report->demo_cell.y, &demo);
    core_rasterize(core, &raster_on_pixels);

    report->raster_on_pixels = raster_on_pixels;
    report->cell_hash = fnv1a_u32(&core->cells[report->demo_cell.y][report->demo_cell.x], sizeof(Cell));
    report->framebuffer_hash = fnv1a_u32(core->pixels, sizeof(core->pixels));
    report->self_check_passed = report->glyph_on_pixels == report->raster_on_pixels &&
        report->expected_brightness == demo.brightness;
}

int core_write_headless_dump(const RenderCore *core, const HeadlessReport *report, FILE *out) {
    const Cell *demo = &core->cells[report->demo_cell.y][report->demo_cell.x];
    int row;

    if (out == NULL) {
        return -1;
    }

    fprintf(out, "mode=headless\n");
    fprintf(out, "framebuffer=%dx%d\n", SLICE1_FB_WIDTH, SLICE1_FB_HEIGHT);
    fprintf(out, "grid=%dx%d\n", SLICE1_GRID_WIDTH, SLICE1_GRID_HEIGHT);
    fprintf(out, "demo_cell=%d,%d\n", report->demo_cell.x, report->demo_cell.y);
    fprintf(out, "brightness=%u\n", (unsigned)demo->brightness);
    fprintf(out, "fg_idx=%u\n", (unsigned)demo->fg_index);
    fprintf(out, "bg_idx=%u\n", (unsigned)demo->bg_index);
    fprintf(out, "glyph_on_pixels=%u\n", report->glyph_on_pixels);
    fprintf(out, "raster_on_pixels=%u\n", report->raster_on_pixels);
    fprintf(out, "cell_hash=0x%08X\n", report->cell_hash);
    fprintf(out, "framebuffer_hash=0x%08X\n", report->framebuffer_hash);
    fprintf(out, "demo_rows=");

    for (row = 0; row < SLICE1_CELL_HEIGHT; ++row) {
        fprintf(out, "%s%02X", row == 0 ? "" : ",", demo->glyph.rows[row]);
    }

    fprintf(out, "\n");
    fprintf(out, "self_check=%s\n", report->self_check_passed ? "PASS" : "FAIL");

    return ferror(out) ? -1 : 0;
}
