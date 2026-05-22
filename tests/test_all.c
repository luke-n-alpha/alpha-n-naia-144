#include "engine/render/render.h"
#include "engine/input/input.h"
#include "engine/font/font.h"
#include "game/scene.h"
#include "game/dialog.h"
#include "game/battle.h"
#include "game/save.h"
#include <stdio.h>
#include <string.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) do { printf("  %-40s", name); } while(0)
#define PASS() do { printf("PASS\n"); tests_passed++; } while(0)
#define FAIL(msg) do { printf("FAIL: %s\n", msg); tests_failed++; } while(0)

static void test_render_init(void) {
    TEST("render init/shutdown");
    if (render_init(RENDER_TEXT) != 0) { FAIL("init failed"); return; }
    render_shutdown();
    PASS();
}

static void test_text_putc(void) {
    TEST("text_putc within bounds");
    if (render_init(RENDER_TEXT) != 0) { FAIL("init"); return; }
    int r = text_putc(0, 0, 'A', 7, 0);
    render_shutdown();
    if (r == 0) PASS(); else FAIL("put returned error");
}

static void test_text_putc_oob(void) {
    TEST("text_putc out of bounds");
    if (render_init(RENDER_TEXT) != 0) { FAIL("init"); return; }
    int r = text_putc(-1, 0, 'X', 7, 0);
    render_shutdown();
    if (r != 0) PASS(); else FAIL("should fail OOB");
}

static void test_text_puts_utf8(void) {
    TEST("text_puts Korean UTF-8");
    if (render_init(RENDER_TEXT) != 0) { FAIL("init"); return; }
    int r = text_puts(0, 0, "한글", 15, 0);
    render_shutdown();
    if (r == 0) PASS(); else FAIL("puts failed");
}

static void test_render_size(void) {
    TEST("render_size TEXT mode");
    render_init(RENDER_TEXT);
    render_set_mode(RENDER_TEXT);
    ScreenSize s = render_size();
    render_shutdown();
    if (s.width == 80 && s.height == 25) PASS();
    else FAIL("unexpected size");
}

static void test_pixel_set(void) {
    TEST("pixel_set within bounds");
    if (render_init(RENDER_TEXT) != 0) { FAIL("init"); return; }
    int r = pixel_set(0, 0, 7);
    render_shutdown();
    if (r == 0) PASS(); else FAIL("pixel_set error");
}

static void test_font_glyph(void) {
    TEST("font_get_glyph ASCII");
    const GlyphBitmap *g = font_get_glyph('A');
    if (g != NULL) PASS(); else FAIL("null glyph");
}

static void test_font_glyph_space(void) {
    TEST("font_get_glyph space");
    const GlyphBitmap *g = font_get_glyph(' ');
    if (g != NULL) PASS(); else FAIL("null glyph");
}

static void test_font_glyph_unknown(void) {
    TEST("font_get_glyph unknown -> fallback");
    const GlyphBitmap *g = font_get_glyph(0xFFFF);
    if (g != NULL) PASS(); else FAIL("null on unknown");
}

static void test_scene_transitions(void) {
    TEST("scene transitions TITLE->DIALOG->CAFE");
    render_init(RENDER_TEXT);
    input_init();
    scene_enter(SCENE_TITLE);
    if (scene_current() != SCENE_TITLE) { FAIL("not title"); return; }

    scene_handle_key(KEY_CONFIRM);
    if (scene_current() != SCENE_DIALOG) { FAIL("not dialog after confirm"); return; }

    while (dialog_advance() == 0) {}
    scene_handle_key(KEY_CONFIRM);
    if (scene_current() != SCENE_CAFE) { FAIL("not cafe after dialog"); return; }

    input_shutdown();
    render_shutdown();
    PASS();
}

static void test_dialog_all(void) {
    TEST("all dialog entries valid");
    int ok = 1;
    for (int i = 0; i < 24; i++) {
        if (dialog_start(i) != 0) { ok = 0; break; }
        int safety = 0;
        while (dialog_advance() == 0 && safety < 20) safety++;
    }
    if (ok) PASS(); else FAIL("dialog start failed");
}

static void test_save_roundtrip(void) {
    TEST("save write/read roundtrip");
    const char *data = "ch:2,nr:45,at:12";
    save_erase(0);
    if (save_write(0, data, strlen(data)) != 0) { FAIL("write"); return; }
    char buf[256];
    memset(buf, 'X', sizeof(buf));
    int len = save_read(0, buf, sizeof(buf));
    save_erase(0);
    if (len < 0) { FAIL("read"); return; }
    if (strcmp(buf, data) == 0) PASS(); else FAIL("mismatch");
}

static void test_save_exact_fit_nul(void) {
    TEST("save_read exact-fit stays NUL terminated");
    char data[16];
    char buf[8];
    int len;
    int i;

    for (i = 0; i < (int)sizeof(data); i++) data[i] = 'A' + i;

    save_erase(1);
    if (save_write(1, data, sizeof(buf)) != 0) { FAIL("write"); return; }

    memset(buf, 'X', sizeof(buf));
    len = save_read(1, buf, sizeof(buf));
    save_erase(1);

    if (len != (int)sizeof(buf) - 1) { FAIL("unexpected length"); return; }
    if (buf[sizeof(buf) - 1] != '\0') { FAIL("missing NUL"); return; }
    PASS();
}

static void test_save_oob(void) {
    TEST("save OOB slot rejected");
    if (save_write(-1, "x", 1) != 0 && save_write(3, "x", 1) != 0)
        PASS();
    else FAIL("should reject");
}

static void test_jamo_brightness(void) {
    TEST("JAMO_BRIGHTNESS all values 0-15");
    int ok = 1;
    for (int i = 0; i < GLYPH_JAMO_COUNT; i++) {
        if (JAMO_BRIGHTNESS[i] > 15) { ok = 0; break; }
    }
    if (ok) PASS(); else FAIL("brightness OOB");
}

static void test_battle_start(void) {
    TEST("battle start/update cycle");
    battle_start(1);
    int done = 0;
    for (int i = 0; i < 2000; i++) {
        battle_handle_key(KEY_UP);
        battle_handle_key(KEY_DOWN);
        battle_handle_key(KEY_LEFT);
        battle_handle_key(KEY_RIGHT);
        if (battle_update(100)) { done = 1; break; }
    }
    if (done) PASS(); else FAIL("battle never completes");
}

static void test_game_full_run(void) {
    TEST("full game playthrough (auto)");
    render_init(RENDER_TEXT);
    input_init();
    memset(&g_state, 0, sizeof(g_state));

    scene_enter(SCENE_TITLE);
    scene_handle_key(KEY_CONFIRM);

    while (scene_current() == SCENE_DIALOG)
        scene_handle_key(KEY_CONFIRM);

    int chapter_reached = g_state.chapter;
    int ending_reached = 0;
    int ending_id = -1;

    for (int attempt = 0; attempt < 10000; attempt++) {
        SceneType sc = scene_current();
        if (g_state.chapter > chapter_reached) chapter_reached = g_state.chapter;
        if (sc == SCENE_ENDING) {
            ending_reached = 1;
            ending_id = g_state.current_ending;
            scene_handle_key(KEY_CONFIRM);
            break;
        }
        if (sc == SCENE_TITLE) break;

        if (sc == SCENE_CAFE) {
            int step;
            for (step = 0; step < 4 && scene_current() == SCENE_CAFE; step++) {
                scene_handle_key(KEY_CONFIRM);
                if (scene_current() == SCENE_CAFE)
                    scene_handle_key(KEY_DOWN);
            }
        } else if (sc == SCENE_DIALOG) {
            scene_handle_key(KEY_CONFIRM);
        } else if (sc == SCENE_BATTLE) {
            scene_handle_key(KEY_UP);
            scene_handle_key(KEY_DOWN);
            scene_handle_key(KEY_LEFT);
            scene_handle_key(KEY_RIGHT);
            scene_update(100);
        } else if (sc == SCENE_ATELIER) {
            scene_handle_key(g_state.chapter >= 3 ? KEY_CONFIRM : KEY_CANCEL);
        } else {
            break;
        }
    }

    input_shutdown();
    render_shutdown();
    if (ending_reached &&
        scene_current() == SCENE_TITLE &&
        chapter_reached >= 4 &&
        ending_id >= 0 && ending_id <= 2)
        PASS();
    else
        FAIL("game did not complete");
}

int main(void) {
    printf("\n=== alpha-n-naia-144 QA ===\n\n");

    printf("[Render]\n");
    test_render_init();
    test_text_putc();
    test_text_putc_oob();
    test_text_puts_utf8();
    test_render_size();
    test_pixel_set();

    printf("\n[Font]\n");
    test_font_glyph();
    test_font_glyph_space();
    test_font_glyph_unknown();
    test_jamo_brightness();

    printf("\n[Dialog]\n");
    test_dialog_all();

    printf("\n[Scene]\n");
    test_scene_transitions();

    printf("\n[Battle]\n");
    test_battle_start();

    printf("\n[Save]\n");
    test_save_roundtrip();
    test_save_exact_fit_nul();
    test_save_oob();

    printf("\n[Integration]\n");
    test_game_full_run();

    printf("\n=== Results: %d passed, %d failed ===\n\n", tests_passed, tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
