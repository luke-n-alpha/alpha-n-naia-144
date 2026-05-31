#include "game.h"
#include "font.h"

/* Deterministic LCG so FR3 brightness is reproducible from --seed (NFR7). */
static unsigned int g_rng;
static void rng_seed(unsigned int s) { g_rng = s ? s : 1u; }
static unsigned int rng_next(void) {
    g_rng = g_rng * 1103515245u + 12345u;
    return (g_rng >> 16) & 0x7fffu;
}

/* FR1: CAFE title "CAFELUA" near top-center (full scene = later content bucket). */
static void draw_title(RenderCore *core) {
    int ids[7];
    ids[0] = FONT_GLYPH_LATIN_C; ids[1] = FONT_GLYPH_LATIN_A; ids[2] = FONT_GLYPH_LATIN_F;
    ids[3] = FONT_GLYPH_LATIN_E; ids[4] = FONT_GLYPH_LATIN_L; ids[5] = FONT_GLYPH_LATIN_U;
    ids[6] = FONT_GLYPH_LATIN_A;
    font_draw_glyphs(core, 36, 4, ids, 7, 12, 7, 0);
}

/* FR3: 5 jamo tiles on row 12 at x=38,40,42,44,46; current = RNG, target fixed. */
static void build_tiles(GameState *g) {
    static const int glyphs[GAME_MAX_TILES] = {
        FONT_GLYPH_GIYEOK, FONT_GLYPH_NIEUN, FONT_GLYPH_DIGEUT,
        FONT_GLYPH_RIEUL, FONT_GLYPH_MIEUM
    };
    int i;
    g->tile_count = GAME_MAX_TILES;
    for (i = 0; i < GAME_MAX_TILES; ++i) {
        g->tiles[i].pos.x = 38 + i * 2;
        g->tiles[i].pos.y = 12;
        g->tiles[i].glyph_id = glyphs[i];
        g->tiles[i].current = (uint8_t)(rng_next() % 16u);
        g->tiles[i].target = (uint8_t)((i * 5 + 3) % 16);
    }
}

static void render_tiles(GameState *g) {
    int i;
    for (i = 0; i < g->tile_count; ++i) {
        int id = g->tiles[i].glyph_id;
        font_draw_glyphs(&g->core, g->tiles[i].pos.x, g->tiles[i].pos.y,
                         &id, 1, g->tiles[i].current, 7, 0);
    }
}

/* FR5: when every tile current == target, latch the restore flag + dialogue.
 * Latch-once (mutation-resistant): only a fully-correct alignment sets it. */
static void check_restore(GameState *g) {
    int i;
    for (i = 0; i < g->tile_count; ++i) {
        if (g->tiles[i].current != g->tiles[i].target) return;
    }
    g->restore_flag = 1;
    g->dialogue = "\xea\xb8\xb0\xec\x96\xb5\xec\x9d\xb4 \xeb\x8f\x8c\xec\x95\x84\xec\x99\x94\xeb\x8b\xa4"; /* "기억이 돌아왔다" */
}

void game_init_seeded(GameState *g, unsigned int seed) {
    core_init(&g->core);
    core_clear_cells(&g->core, 0);
    draw_title(&g->core);
    rng_seed(seed);
    build_tiles(g);
    render_tiles(g);
    g->cursor.x = 40;   /* on tile index 1 */
    g->cursor.y = 12;
    g->restore_flag = 0;
    g->dialogue = NULL;
    g->initialized = 1;
}

void game_init(GameState *g) { game_init_seeded(g, 1u); }

static int tile_under_cursor(const GameState *g) {
    int i;
    for (i = 0; i < g->tile_count; ++i) {
        if (g->tiles[i].pos.x == g->cursor.x && g->tiles[i].pos.y == g->cursor.y) {
            return i;
        }
    }
    return -1;
}

void game_handle_key(GameState *g, GameKey key) {
    switch (key) {
        case GAME_KEY_UP:    if (g->cursor.y > 0) g->cursor.y--; break;
        case GAME_KEY_DOWN:  if (g->cursor.y < SLICE1_GRID_HEIGHT - 1) g->cursor.y++; break;
        case GAME_KEY_LEFT:  if (g->cursor.x > 0) g->cursor.x--; break;
        case GAME_KEY_RIGHT: if (g->cursor.x < SLICE1_GRID_WIDTH - 1) g->cursor.x++; break;
        case GAME_KEY_CONFIRM: {   /* FR4: cycle brightness; FR5: check win after */
            int t = tile_under_cursor(g);
            if (t >= 0) {
                g->tiles[t].current = (uint8_t)((g->tiles[t].current + 1u) % 16u);
                render_tiles(g);
                check_restore(g);
            }
            break;
        }
        case GAME_KEY_CANCEL:
        default:
            break;
    }
}

static int count_filled_cells(const RenderCore *core) {
    int n = 0, x, y, r;
    for (y = 0; y < SLICE1_GRID_HEIGHT; ++y) {
        for (x = 0; x < SLICE1_GRID_WIDTH; ++x) {
            int on = 0;
            for (r = 0; r < SLICE1_CELL_HEIGHT; ++r) {
                if (core->cells[y][x].glyph.rows[r]) { on = 1; break; }
            }
            if (on) n++;
        }
    }
    return n;
}

int game_dump(const GameState *g, FILE *out) {
    int i, t;
    if (fprintf(out, "grid_w=%d\n", SLICE1_GRID_WIDTH) < 0) return 1;
    fprintf(out, "grid_h=%d\n", SLICE1_GRID_HEIGHT);
    fprintf(out, "cursor_x=%d\n", g->cursor.x);
    fprintf(out, "cursor_y=%d\n", g->cursor.y);
    fprintf(out, "filled_cells=%d\n", count_filled_cells(&g->core));
    fprintf(out, "tile_count=%d\n", g->tile_count);
    for (i = 0; i < g->tile_count; ++i) {
        fprintf(out, "tile%d=%d,%d,%u,%u\n", i, g->tiles[i].pos.x, g->tiles[i].pos.y,
                g->tiles[i].current, g->tiles[i].target);
    }
    t = tile_under_cursor(g);
    fprintf(out, "sel_brightness=%d\n", t >= 0 ? (int)g->tiles[t].current : -1);
    fprintf(out, "restore_flag=%d\n", g->restore_flag);
    fprintf(out, "dialogue=%s\n", g->dialogue ? g->dialogue : "-");
    fprintf(out, "game_dump=PASS\n");
    return 0;
}
