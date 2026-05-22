#ifndef INPUT_H
#define INPUT_H

typedef enum {
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_CONFIRM, KEY_CANCEL, KEY_MENU,
    KEY_NONE,
    KEY_COUNT
} GameKey;

int input_init(void);
GameKey input_poll(void);
void input_shutdown(void);

#endif
