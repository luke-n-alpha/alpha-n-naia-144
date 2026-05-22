#include "input.h"
#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static struct termios orig_term;

int input_init(void) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_term);
    raw = orig_term;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    return 0;
}

static int read_byte(void) {
    uint8_t c = 0;
    int n = read(STDIN_FILENO, &c, 1);
    return n > 0 ? c : -1;
}

GameKey input_poll(void) {
    int c = read_byte();
    if (c < 0) return KEY_NONE;
    if (c == 27) {
        int b1 = read_byte();
        int b2 = read_byte();
        if (b1 == '[') {
            switch (b2) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
            }
        }
        return KEY_CANCEL;
    }
    if (c == '\n' || c == '\r' || c == ' ') return KEY_CONFIRM;
    if (c == 'q' || c == 127) return KEY_CANCEL;
    if (c == 'm' || c == 'M') return KEY_MENU;
    return KEY_NONE;
}

void input_shutdown(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
}
