#ifndef SCENE_H
#define SCENE_H

#include "../engine/input/input.h"
#include <stdint.h>

typedef enum {
    SCENE_TITLE,
    SCENE_CAFE,
    SCENE_ATELIER,
    SCENE_BATTLE,
    SCENE_DIALOG,
    SCENE_ENDING
} SceneType;

typedef struct {
    int chapter;
    int season;
    int alpha_trust;
    int naia_restore;
    int dialog_attitude;
    int cafe_explored[4];
    int puzzles_solved;
    int current_ending;
} GameState;

extern GameState g_state;

int scene_enter(SceneType scene);
int scene_exit(void);
int scene_update(uint32_t delta_ms);
GameKey scene_handle_key(GameKey key);
SceneType scene_current(void);

#endif
