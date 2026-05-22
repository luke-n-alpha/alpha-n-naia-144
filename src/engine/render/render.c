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

static int utf8_decode(const char **utf8, uint32_t *out_cp) {
    const uint8_t *p = (const uint8_t *)*utf8;

    if (!p[0]) return 0;

    if (p[0] < 0x80) {
        *out_cp = p[0];
        *utf8 += 1;
        return 1;
    }
    if ((p[0] & 0xE0) == 0xC0 && p[1]) {
        *out_cp = ((uint32_t)(p[0] & 0x1F) << 6) | (uint32_t)(p[1] & 0x3F);
        *utf8 += 2;
        return 1;
    }
    if ((p[0] & 0xF0) == 0xE0 && p[1] && p[2]) {
        *out_cp = ((uint32_t)(p[0] & 0x0F) << 12) |
                  ((uint32_t)(p[1] & 0x3F) << 6) |
                  (uint32_t)(p[2] & 0x3F);
        *utf8 += 3;
        return 1;
    }
    if ((p[0] & 0xF8) == 0xF0 && p[1] && p[2] && p[3]) {
        *out_cp = ((uint32_t)(p[0] & 0x07) << 18) |
                  ((uint32_t)(p[1] & 0x3F) << 12) |
                  ((uint32_t)(p[2] & 0x3F) << 6) |
                  (uint32_t)(p[3] & 0x3F);
        *utf8 += 4;
        return 1;
    }

    *utf8 += 1;
    return -1;
}

static int decompose_hangul_compat(uint32_t cp, uint32_t out[3]) {
    static const uint16_t lead_map[19] = {
        0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141, 0x3142, 0x3143,
        0x3145, 0x3146, 0x3147, 0x3148, 0x3149, 0x314A, 0x314B, 0x314C, 0x314D,
        0x314E
    };
    static const uint16_t vowel_map[21] = {
        0x314F, 0x3150, 0x3151, 0x3152, 0x3153, 0x3154, 0x3155, 0x3156, 0x3157,
        0x3158, 0x3159, 0x315A, 0x315B, 0x315C, 0x315D, 0x315E, 0x315F, 0x3160,
        0x3161, 0x3162, 0x3163
    };
    static const uint16_t tail_map[28] = {
        0x0000, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136, 0x3137, 0x3139,
        0x313A, 0x313B, 0x313C, 0x313D, 0x313E, 0x313F, 0x3140, 0x3141, 0x3142,
        0x3144, 0x3145, 0x3146, 0x3147, 0x3148, 0x314A, 0x314B, 0x314C, 0x314D,
        0x314E
    };
    uint32_t s_index;
    uint32_t lead_idx;
    uint32_t vowel_idx;
    uint32_t tail_idx;

    if (cp < 0xAC00 || cp > 0xD7A3) return 0;

    s_index = cp - 0xAC00;
    lead_idx = s_index / 588;
    vowel_idx = (s_index % 588) / 28;
    tail_idx = s_index % 28;

    out[0] = lead_map[lead_idx];
    out[1] = vowel_map[vowel_idx];
    if (tail_idx) {
        out[2] = tail_map[tail_idx];
        return 3;
    }
    return 2;
}

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
    uint32_t cp;

    if (!utf8) return -1;
    {
        int c = col;
    while (*utf8 && c < SCREEN_COLS) {
        uint32_t jamo[3];
        int n;

        n = utf8_decode(&utf8, &cp);
        if (n <= 0) continue;

        n = decompose_hangul_compat(cp, jamo);
        if (n > 0) {
            int i;
            for (i = 0; i < n && c < SCREEN_COLS; i++, c++) {
                text_putc(row, c, jamo[i], fg, bg);
            }
            continue;
        }

        text_putc(row, c, cp, fg, bg);
        c++;
    }
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
