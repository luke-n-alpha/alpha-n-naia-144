#include "engine/render/render.h"
#include "engine/input/input.h"
#include "game/scene.h"
#include "game/dialog.h"
#include "game/battle.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

int main(void) {
    render_init();
    memset(&g_state, 0, sizeof(g_state));
    srand(42);

    printf("\n=== SCREENSHOT CAPTURE ===\n\n");

    printf("--- [1] TITLE SCREEN ---\n");
    scene_enter(SCENE_TITLE);
    scene_update(0);
    render_flush();

    printf("\n--- [2] INTRO DIALOG ---\n");
    dialog_start(0);
    scene_enter(SCENE_DIALOG);
    scene_update(0);
    render_flush();

    printf("\n--- [3] CAFE (Spring, nothing explored) ---\n");
    scene_enter(SCENE_CAFE);
    scene_update(0);
    render_flush();

    printf("\n--- [4] CAFE (after counter dialog) ---\n");
    g_state.cafe_explored[0] = 1;
    g_state.puzzles_solved = 1;
    g_state.naia_restore = 8;
    g_state.alpha_trust = 3;
    scene_enter(SCENE_CAFE);
    scene_update(0);
    render_flush();

    printf("\n--- [5] COUNTER DIALOG ---\n");
    dialog_start(1);
    scene_enter(SCENE_DIALOG);
    scene_update(0);
    render_flush();

    printf("\n--- [6] BATTLE (rhythm puzzle) ---\n");
    g_state.chapter = 2;
    scene_enter(SCENE_BATTLE);
    battle_start(3);
    battle_update(500);
    battle_update(500);
    battle_update(500);
    battle_draw();
    render_flush();

    printf("\n--- [7] ATELIER ---\n");
    g_state.chapter = 2;
    scene_enter(SCENE_ATELIER);
    scene_update(0);
    render_flush();

    printf("\n--- [8] ENDING A (일상 회복) ---\n");
    g_state.current_ending = 0;
    g_state.naia_restore = 70;
    g_state.alpha_trust = 25;
    scene_enter(SCENE_ENDING);
    scene_update(0);
    render_flush();

    printf("\n--- [9] ENDING B (기억 공명) ---\n");
    g_state.current_ending = 1;
    g_state.naia_restore = 50;
    g_state.alpha_trust = 15;
    scene_enter(SCENE_ENDING);
    scene_update(0);
    render_flush();

    printf("\n=== ALL SCREENS CAPTURED ===\n");

    render_shutdown();
    return 0;
}
