#include "platform.h"

#include <stddef.h>

int plat_init(Platform *plat, const PlatformConfig *config) {
    plat->impl = NULL;
    (void)config;
    return 0;
}

int plat_present(Platform *plat, const uint32_t *pixels, int width, int height) {
    (void)plat;
    (void)pixels;
    (void)width;
    (void)height;
    return 0;
}

int plat_poll(Platform *plat, int *should_quit) {
    (void)plat;
    if (should_quit != NULL) {
        *should_quit = 1;
    }
    return 0;
}

PlatKey plat_poll_key(Platform *plat) {
    (void)plat;
    return PLAT_KEY_NONE;
}

void plat_shutdown(Platform *plat) {
    (void)plat;
}
