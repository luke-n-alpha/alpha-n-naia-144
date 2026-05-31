#ifndef SLICE1_V2_PLATFORM_H
#define SLICE1_V2_PLATFORM_H

#include <stdint.h>

typedef struct {
    void *impl;
} Platform;

typedef struct {
    int width;
    int height;
    const char *title;
} PlatformConfig;

/* P3: abstract key (platform-independent) so game logic never sees keysyms. */
typedef enum {
    PLAT_KEY_NONE = 0,
    PLAT_KEY_UP, PLAT_KEY_DOWN, PLAT_KEY_LEFT, PLAT_KEY_RIGHT,
    PLAT_KEY_CONFIRM, PLAT_KEY_CANCEL, PLAT_KEY_QUIT
} PlatKey;

int plat_init(Platform *plat, const PlatformConfig *config);
int plat_present(Platform *plat, const uint32_t *pixels, int width, int height);
int plat_poll(Platform *plat, int *should_quit);
/* P3: next pending key as a PlatKey (PLAT_KEY_NONE if none, PLAT_KEY_QUIT on close). */
PlatKey plat_poll_key(Platform *plat);
void plat_shutdown(Platform *plat);

#endif
