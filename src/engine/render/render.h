#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

#define SCREEN_COLS 80
#define SCREEN_ROWS 25
#define PALETTE_SIZE 16

typedef enum {
    RENDER_TEXT,
    RENDER_PIXEL
} RenderMode;

typedef struct {
    int width;
    int height;
} ScreenSize;

int render_init(void);
int render_set_mode(RenderMode mode);
int render_flush(void);
int render_clear(void);
ScreenSize render_size(void);
void render_shutdown(void);

int text_putc(int row, int col, uint32_t ch, uint8_t fg, uint8_t bg);
int text_puts(int row, int col, const char *utf8, uint8_t fg, uint8_t bg);
int text_fill(int row, int col, int w, int h, uint32_t ch, uint8_t fg, uint8_t bg);

int pixel_set(int x, int y, uint8_t color_idx);
int pixel_fill_rect(int x, int y, int w, int h, uint8_t color_idx);

extern const uint8_t PALETTE_R[PALETTE_SIZE];
extern const uint8_t PALETTE_G[PALETTE_SIZE];
extern const uint8_t PALETTE_B[PALETTE_SIZE];

#endif
