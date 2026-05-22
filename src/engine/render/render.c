#include "render.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define VBUF_SIZE (SCREEN_COLS * SCREEN_ROWS)

typedef struct {
    uint32_t ch;
    uint8_t  fg;
    uint8_t  bg;
} TextCell;

static TextCell vbuf[2][VBUF_SIZE];
static int active_buf = 0;
static RenderMode current_mode = RENDER_TEXT;

const uint8_t PALETTE_R[16] = {
    0,0,0,0, 128,128,128,128, 192,192,192,192, 255,255,255,255
};
const uint8_t PALETTE_G[16] = {
    0,0,128,128, 0,0,128,128, 0,0,128,128, 0,0,128,128
};
const uint8_t PALETTE_B[16] = {
    0,128,0,128, 0,128,0,128, 0,128,0,128, 0,128,0,128
};

static void ansi_color(uint8_t fg, uint8_t bg) {
    printf("\033[38;5;%dm\033[48;5;%dm",
           fg < 8 ? fg : fg + 82,
           bg < 8 ? bg : bg + 82);
}

static TextCell *cell_at(TextCell *buf, int row, int col) {
    if (row < 0 || row >= SCREEN_ROWS || col < 0 || col >= SCREEN_COLS) return NULL;
    return &buf[row * SCREEN_COLS + col];
}

typedef struct {
    uint8_t sub[16][8];
} PixelCell;

static PixelCell pixel_buf[SCREEN_ROWS][SCREEN_COLS];
static int pixel_dirty[SCREEN_ROWS][SCREEN_COLS];

int render_init(RenderMode mode) {
    printf("\033[?25l");
    printf("\033[2J");
    memset(vbuf, 0, sizeof(vbuf));
    memset(pixel_buf, 0, sizeof(pixel_buf));
    memset(pixel_dirty, 0, sizeof(pixel_dirty));
    active_buf = 0;
    current_mode = mode;
    return 0;
}

int render_set_mode(RenderMode mode) {
    if (mode != RENDER_TEXT && mode != RENDER_PIXEL) return -1;
    if (mode == RENDER_PIXEL && current_mode != RENDER_PIXEL)
        memset(pixel_buf, 0, sizeof(pixel_buf));
    current_mode = mode;
    return 0;
}

int render_clear(void) {
    TextCell *buf = vbuf[active_buf];
    for (int i = 0; i < VBUF_SIZE; i++) {
        buf[i].ch = ' ';
        buf[i].fg = 7;
        buf[i].bg = 0;
    }
    return 0;
}

int render_flush(void) {
    TextCell *front = vbuf[active_buf];
    TextCell *back  = vbuf[1 - active_buf];

    if (current_mode == RENDER_PIXEL) {
        for (int r = 0; r < SCREEN_ROWS; r++) {
            for (int c = 0; c < SCREEN_COLS; c++) {
                if (!pixel_dirty[r][c]) continue;
                pixel_dirty[r][c] = 0;
                PixelCell *pc = &pixel_buf[r][c];
                printf("\033[%d;%dH", r + 1, c + 1);
                uint8_t dominant = pc->sub[0][0];
                unsigned int sum = 0;
                int count = 0;
                for (int sy = 0; sy < 16; sy++) {
                    for (int sx = 0; sx < 8; sx++) {
                        if (pc->sub[sy][sx] == dominant) count++;
                        sum += pc->sub[sy][sx];
                    }
                }
                uint8_t avg = (uint8_t)(sum / 128);
                if (count == 128) {
                    ansi_color(dominant, dominant);
                } else {
                    ansi_color(avg, avg);
                }
                putchar(0xE2); putchar(0x96); putchar(0x88);
            }
        }
    } else {
        printf("\033[H");
        for (int r = 0; r < SCREEN_ROWS; r++) {
            for (int c = 0; c < SCREEN_COLS; c++) {
                TextCell *f = &front[r * SCREEN_COLS + c];
                TextCell *b = &back[r * SCREEN_COLS + c];
                if (f->fg != b->fg || f->bg != b->bg) ansi_color(f->fg, f->bg);
                if (f->ch != b->ch) {
                    printf("\033[%d;%dH", r + 1, c + 1);
                    ansi_color(f->fg, f->bg);
                    if (f->ch < 128) {
                        putchar(f->ch);
                    } else {
                        char mb[5] = {0};
                        if (f->ch < 0x800) {
                            mb[0] = 0xC0 | (f->ch >> 6);
                            mb[1] = 0x80 | (f->ch & 0x3F);
                        } else if (f->ch < 0x10000) {
                            mb[0] = 0xE0 | (f->ch >> 12);
                            mb[1] = 0x80 | ((f->ch >> 6) & 0x3F);
                            mb[2] = 0x80 | (f->ch & 0x3F);
                        }
                        fputs(mb, stdout);
                    }
                }
            }
        }
    }
    fflush(stdout);
    active_buf = 1 - active_buf;
    return 0;
}

ScreenSize render_size(void) {
    ScreenSize s;
    if (current_mode == RENDER_TEXT) {
        s.width = SCREEN_COLS;
        s.height = SCREEN_ROWS;
    } else {
        s.width = SCREEN_COLS * 8;
        s.height = SCREEN_ROWS * 16;
    }
    return s;
}

void render_shutdown(void) {
    printf("\033[0m\033[2J\033[H\033[?25h");
    fflush(stdout);
}

int text_putc(int row, int col, uint32_t ch, uint8_t fg, uint8_t bg) {
    TextCell *c = cell_at(vbuf[active_buf], row, col);
    if (!c) return -1;
    c->ch = ch;
    c->fg = fg;
    c->bg = bg;
    return 0;
}

int text_puts(int row, int col, const char *utf8, uint8_t fg, uint8_t bg) {
    if (!utf8) return -1;
    int c = col;
    while (*utf8 && c < SCREEN_COLS) {
        uint32_t cp = 0;
        const uint8_t *p = (const uint8_t *)utf8;
        if (p[0] < 0x80) { cp = p[0]; utf8 += 1; }
        else if ((p[0] & 0xE0) == 0xC0) { cp = (p[0] & 0x1F) << 6 | (p[1] & 0x3F); utf8 += 2; }
        else if ((p[0] & 0xF0) == 0xE0) { cp = (p[0] & 0x0F) << 12 | (p[1] & 0x3F) << 6 | (p[2] & 0x3F); utf8 += 3; }
        else { utf8 += 1; continue; }
        text_putc(row, c, cp, fg, bg);
        c++;
    }
    return 0;
}

int text_fill(int row, int col, int w, int h, uint32_t ch, uint8_t fg, uint8_t bg) {
    for (int r = row; r < row + h && r < SCREEN_ROWS; r++)
        for (int c = col; c < col + w && c < SCREEN_COLS; c++)
            text_putc(r, c, ch, fg, bg);
    return 0;
}

int pixel_set(int x, int y, uint8_t color_idx) {
    if (color_idx >= PALETTE_SIZE) return -1;
    int col = x / 8;
    int row = y / 16;
    if (row < 0 || row >= SCREEN_ROWS || col < 0 || col >= SCREEN_COLS) return -1;
    int sx = x % 8;
    int sy = y % 16;
    pixel_buf[row][col].sub[sy][sx] = color_idx;
    pixel_dirty[row][col] = 1;
    return 0;
}

int pixel_fill_rect(int x, int y, int w, int h, uint8_t color_idx) {
    if (color_idx >= PALETTE_SIZE) return -1;
    for (int py = y; py < y + h; py++)
        for (int px = x; px < x + w; px++)
            pixel_set(px, py, color_idx);
    return 0;
}
