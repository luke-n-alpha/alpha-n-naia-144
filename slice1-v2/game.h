#ifndef SLICE1_V2_GAME_H
#define SLICE1_V2_GAME_H

/* slice1 P2 game logic CONTRACT (anchor; code must conform).
 * FR1 (80x25 CAFE render) + FR2 (cursor) + FR3 (jamo tiles, RNG brightness) +
 * FR4 (confirm cycles brightness 0->15 wrap). FR5 (win+restore) is next bucket.
 * The headless dump (T0/NFR7) is the deterministic acceptance hook — every
 * FR check reads the dump, no visual judgement needed. */

#include "core.h"

#define GAME_MAX_TILES 5

typedef enum {
    GAME_KEY_UP,
    GAME_KEY_DOWN,
    GAME_KEY_LEFT,
    GAME_KEY_RIGHT,
    GAME_KEY_CONFIRM,
    GAME_KEY_CANCEL
} GameKey;

typedef struct {
    CellPoint pos;
    int glyph_id;
    uint8_t current;   /* FR4 cycles this 0..15 */
    uint8_t target;    /* FR5 goal brightness */
} GameTile;

typedef struct {
    RenderCore core;
    CellPoint cursor;
    GameTile tiles[GAME_MAX_TILES];
    int tile_count;
    int restore_flag;      /* FR5: latched 1 once every tile current == target */
    const char *dialogue;  /* FR5: restore line, NULL until restored */
    int initialized;
} GameState;

/* FR1+FR3: build CAFE scene + jamo tiles with deterministic RNG brightness. */
void game_init_seeded(GameState *g, unsigned int seed);
void game_init(GameState *g);   /* = game_init_seeded(g, 1) */

/* FR2: cursor move (clamped). FR4: CONFIRM cycles brightness of tile under
 *      the cursor (15 -> 0 wrap). CANCEL reserved. */
void game_handle_key(GameState *g, GameKey key);

/* T0/NFR7: deterministic headless dump (cursor + tiles + selected brightness). */
int game_dump(const GameState *g, FILE *out);

#endif
