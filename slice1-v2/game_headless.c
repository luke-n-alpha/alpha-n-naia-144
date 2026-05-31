/* Headless driver for P2 game logic — T0/NFR7 deterministic acceptance hook.
 * --seed N selects the FR3 RNG seed; --script "RRRDC" injects keys.
 * Letters: U/D/L/R = arrows, C = confirm, X = cancel; others ignored. */
#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_key(char c, GameKey *out) {
    switch (c) {
        case 'U': *out = GAME_KEY_UP; return 1;
        case 'D': *out = GAME_KEY_DOWN; return 1;
        case 'L': *out = GAME_KEY_LEFT; return 1;
        case 'R': *out = GAME_KEY_RIGHT; return 1;
        case 'C': *out = GAME_KEY_CONFIRM; return 1;
        case 'X': *out = GAME_KEY_CANCEL; return 1;
        default: return 0;
    }
}

int main(int argc, char **argv) {
    GameState g;
    const char *script = "";
    unsigned int seed = 1u;
    int i;
    GameKey key;

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--script") == 0 && i + 1 < argc) {
            script = argv[++i];
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (unsigned int)strtoul(argv[++i], NULL, 10);
        } else {
            fprintf(stderr, "unknown arg: %s\n", argv[i]);
            return 64;
        }
    }

    game_init_seeded(&g, seed);

    for (i = 0; script[i] != '\0'; ++i) {
        if (parse_key(script[i], &key)) {
            game_handle_key(&g, key);
        }
    }

    return game_dump(&g, stdout);
}
