#include "core.h"
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int run_headless(FILE *out) {
    RenderCore core;
    HeadlessReport report;

    core_init(&core);
    core_build_demo(&core, &report);
    if (core_write_headless_dump(&core, &report, out) != 0) {
        return 1;
    }

    return report.self_check_passed ? 0 : 2;
}

#ifndef HEADLESS_BUILD
static int run_gui(void) {
    RenderCore core;
    HeadlessReport report;
    Platform platform;
    PlatformConfig config;
    int should_quit = 0;

    core_init(&core);
    core_build_demo(&core, &report);

    config.width = SLICE1_FB_WIDTH;
    config.height = SLICE1_FB_HEIGHT;
    config.title = "alpha-n-naia-144 slice1-v2";

    if (plat_init(&platform, &config) != 0) {
        fprintf(stderr, "plat_init failed\n");
        return 1;
    }

    if (plat_present(&platform, core.pixels, SLICE1_FB_WIDTH, SLICE1_FB_HEIGHT) != 0) {
        fprintf(stderr, "plat_present failed\n");
        plat_shutdown(&platform);
        return 1;
    }

    while (!should_quit) {
        plat_poll(&platform, &should_quit);
    }

    plat_shutdown(&platform);
    return report.self_check_passed ? 0 : 2;
}
#endif

int main(int argc, char **argv) {
#ifndef HEADLESS_BUILD
    int force_headless = 0;
#endif
    const char *dump_path = NULL;
    int i;

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--headless") == 0) {
#ifndef HEADLESS_BUILD
            force_headless = 1;
#endif
            continue;
        } else if (strcmp(argv[i], "--dump") == 0 && i + 1 < argc) {
            dump_path = argv[++i];
        } else {
            fprintf(stderr, "unknown arg: %s\n", argv[i]);
            return 64;
        }
    }

#ifdef HEADLESS_BUILD
    if (dump_path != NULL) {
        FILE *file = fopen(dump_path, "w");
        int rc;
        if (file == NULL) {
            perror("fopen");
            return 1;
        }
        rc = run_headless(file);
        fclose(file);
        return rc;
    }
    return run_headless(stdout);
#else
    if (force_headless) {
        if (dump_path != NULL) {
            FILE *file = fopen(dump_path, "w");
            int rc;
            if (file == NULL) {
                perror("fopen");
                return 1;
            }
            rc = run_headless(file);
            fclose(file);
            return rc;
        }
        return run_headless(stdout);
    }

    return run_gui();
#endif
}
