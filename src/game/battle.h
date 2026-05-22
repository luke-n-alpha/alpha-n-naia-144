#ifndef BATTLE_H
#define BATTLE_H

#include "../engine/input/input.h"
#include <stdint.h>

typedef struct {
    int lane;
    int x;
    int speed;
    int active;
} Note;

int battle_start(int difficulty);
int battle_update(uint32_t delta_ms);
int battle_handle_key(GameKey key);
void battle_draw(void);

#endif
