#ifndef SLICE1_V2_CORE_H
#define SLICE1_V2_CORE_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define SLICE1_FB_WIDTH 640
#define SLICE1_FB_HEIGHT 400
#define SLICE1_GRID_WIDTH 80
#define SLICE1_GRID_HEIGHT 25
#define SLICE1_CELL_WIDTH 8
#define SLICE1_CELL_HEIGHT 16

typedef struct {
    uint8_t rows[SLICE1_CELL_HEIGHT];
} Glyph8x16;

typedef struct {
    Glyph8x16 glyph;
    uint8_t brightness;
    uint8_t fg_index;
    uint8_t bg_index;
} Cell;

typedef struct {
    int x;
    int y;
} CellPoint;

typedef struct {
    uint32_t pixels[SLICE1_FB_WIDTH * SLICE1_FB_HEIGHT];
    Cell cells[SLICE1_GRID_HEIGHT][SLICE1_GRID_WIDTH];
    uint32_t palette[16];
} RenderCore;

typedef struct {
    CellPoint demo_cell;
    uint8_t expected_brightness;
    uint32_t glyph_on_pixels;
    uint32_t raster_on_pixels;
    uint32_t cell_hash;
    uint32_t framebuffer_hash;
    int self_check_passed;
} HeadlessReport;

void core_init(RenderCore *core);
void core_clear_cells(RenderCore *core, uint8_t bg_index);
void core_set_cell(RenderCore *core, int x, int y, const Cell *cell);
void core_rasterize(RenderCore *core, uint32_t *out_on_pixels);
void core_build_demo(RenderCore *core, HeadlessReport *report);
int core_write_headless_dump(const RenderCore *core, const HeadlessReport *report, FILE *out);

#endif
