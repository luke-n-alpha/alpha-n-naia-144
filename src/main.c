#include "engine/render/render.h"
#include "engine/input/input.h"
#include "game/scene.h"
#include <stdio.h>
#include <time.h>

#define FPS 30
#define FRAME_MS (1000 / FPS)

int main(void) {
    if (render_init() != 0) {
        fprintf(stderr, "render init failed\n");
        return 1;
    }
    if (input_init() != 0) {
        fprintf(stderr, "input init failed\n");
        render_shutdown();
        return 1;
    }

    scene_enter(SCENE_TITLE);

    struct timespec prev;
    clock_gettime(CLOCK_MONOTONIC, &prev);

    int running = 1;
    while (running) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        uint32_t delta = (now.tv_sec - prev.tv_sec) * 1000 +
                         (now.tv_nsec - prev.tv_nsec) / 1000000;
        prev = now;

        GameKey key = input_poll();
        if (key == KEY_CANCEL && scene_current() == SCENE_TITLE) {
            running = 0;
        }

        scene_handle_key(key);
        scene_update(delta);
        render_flush();

        if (delta < FRAME_MS) {
            struct timespec sleep = {0, (FRAME_MS - delta) * 1000000L};
            nanosleep(&sleep, NULL);
        }
    }

    input_shutdown();
    render_shutdown();
    return 0;
}
