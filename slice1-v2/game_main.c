/* P3: UC1 GUI shell — game state rendered to a window; arrow/confirm keys drive it.
 * The deterministic logic is already proven by gametest (headless); this is the
 * visual shell (human gate). Run: ./build/naia144_game_x11 (needs DISPLAY). */
#define _DEFAULT_SOURCE  /* expose usleep() under -std=c99 -pedantic */
#include "game.h"
#include "platform.h"

#include <stdio.h>
#include <unistd.h>

static GameKey map_key(PlatKey k) {
    switch (k) {
        case PLAT_KEY_UP: return GAME_KEY_UP;
        case PLAT_KEY_DOWN: return GAME_KEY_DOWN;
        case PLAT_KEY_LEFT: return GAME_KEY_LEFT;
        case PLAT_KEY_RIGHT: return GAME_KEY_RIGHT;
        case PLAT_KEY_CONFIRM: return GAME_KEY_CONFIRM;
        case PLAT_KEY_CANCEL:
        default: return GAME_KEY_CANCEL;
    }
}

int main(void) {
    GameState g;
    Platform plat;
    PlatformConfig cfg;
    int quit = 0;
    int was_restored = 0;

    game_init(&g);

    cfg.width = SLICE1_FB_WIDTH;
    cfg.height = SLICE1_FB_HEIGHT;
    cfg.title = "alpha-n-naia-144 slice1 - CAFELUA";

    if (plat_init(&plat, &cfg) != 0) {
        fprintf(stderr, "plat_init failed (no DISPLAY?)\n");
        return 1;
    }

    core_rasterize(&g.core, NULL);
    plat_present(&plat, g.core.pixels, SLICE1_FB_WIDTH, SLICE1_FB_HEIGHT);

    while (!quit) {
        PlatKey k = plat_poll_key(&plat);
        if (k == PLAT_KEY_QUIT) {
            quit = 1;
        } else if (k != PLAT_KEY_NONE) {
            game_handle_key(&g, map_key(k));
            core_rasterize(&g.core, NULL);
            plat_present(&plat, g.core.pixels, SLICE1_FB_WIDTH, SLICE1_FB_HEIGHT);
            if (g.restore_flag && !was_restored) {
                was_restored = 1;
                printf("restore: %s\n", g.dialogue ? g.dialogue : "-");
                fflush(stdout);
            }
        } else {
            usleep(16000);  /* ~60 fps idle */
        }
    }

    plat_shutdown(&plat);
    return 0;
}
