#include "scene.h"
#include "../engine/render/render.h"
#include "dialog.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

GameState g_state = {0};
static SceneType current_scene = SCENE_TITLE;
static int cursor = 0;

static const char *season_name(int ch) {
    switch (ch) {
        case 0: return "봄";
        case 1: return "여름";
        case 2: return "가을";
        case 3: return "겨울";
    }
    return "?";
}

static uint8_t season_fg(int ch) {
    switch (ch) {
        case 0: return 10;
        case 1: return 14;
        case 2: return 12;
        case 3: return 15;
    }
    return 7;
}

static void draw_title(void) {
    render_clear();
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 8, 0);
    text_puts(4, 15, "자모등 아래의 나이아", 15, 0);
    text_puts(5, 15, "Naia Beneath the Jamo Lights", 8, 0);
    text_puts(8, 15, "1. 새 게임", cursor == 0 ? 15 : 7, 0);
    text_puts(9, 15, "2. 이어하기", cursor == 1 ? 15 : 7, 0);
    text_puts(12, 5, "cafelua x naia  |  1.44MB Disk Game", 3, 0);
    text_puts(14, 5, "Arrow keys: move  |  Space/Enter: confirm  |  Q: back", 8, 0);
    for (int i = 0; i < 40; i++) {
        uint8_t br = (i * 17) % 16;
        text_putc(2, 20 + i, 0x2588, br, 0);
    }
}

static void draw_cafe(void) {
    render_clear();
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 0, 2);
    text_puts(0, 0, "cafelua - ", season_fg(g_state.chapter), 2);
    text_puts(0, 10, season_name(g_state.chapter), season_fg(g_state.chapter), 2);
    text_puts(0, 14, " - 마감 후", 7, 2);

    text_fill(2, 2, 20, 10, ' ', 7, 4);
    text_puts(3, 4, "  카운터  ", 0, 6);
    text_puts(5, 4, "  피아노  ", 0, 6);
    text_puts(7, 4, "  소  파  ", 0, 6);
    text_puts(9, 4, "  창  가  ", 0, 6);

    text_puts(3, 25, "→", 15, 2);
    text_puts(3, 27, cursor == 0 ? "조사" : "이동", 14, 2);
    text_puts(5, 25, "→", 15, 2);
    text_puts(5, 27, cursor == 1 ? "조사" : "이동", 14, 2);

    if (g_state.chapter >= 2) {
        text_puts(9, 50, "[2F 아틀리에]", 12, 2);
    }

    text_puts(23, 0, "방향키:이동  스페이스:조사/대화  M:메뉴", 8, 0);
}

static void draw_battle(void) {
    render_clear();
    render_set_mode(RENDER_PIXEL);
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 0, 1);
    text_puts(0, 0, "리듬 정렬 - ", 15, 1);
    text_puts(0, 12, season_name(g_state.chapter), season_fg(g_state.chapter), 1);

    for (int lane = 0; lane < 4; lane++) {
        int y = 6 + lane * 4;
        text_putc(y, 3, '|', 8, 1);
        text_putc(y, 74, '|', 8, 1);
        for (int x = 4; x < 74; x++) {
            text_putc(y, x, '-', 4, 1);
        }
    }
    text_puts(20, 5, "밝기 파형이 다가옵니다... 타이밍에 맞춰 스페이스!", 14, 1);
    text_puts(22, 5, "Q: 돌아가기", 8, 1);
}

static void draw_ending(void) {
    render_clear();
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 0, 3);
    text_puts(4, 10, "~ 엔 딩 ~", 15, 3);
    const char *ending_text;
    switch (g_state.current_ending) {
        case 0: ending_text = "일상 회복 - 카페에 아침이 돌아왔다."; break;
        case 1: ending_text = "기억 공명 - naia가 빛을 남겼다."; break;
        default: ending_text = "미해결 새벽 - 아직 밤이 끝나지 않았다."; break;
    }
    text_puts(8, 10, ending_text, 14, 3);
    char buf[80];
    snprintf(buf, sizeof(buf), "Alpha 신뢰도: %d  |  naia 복원률: %d%%",
             g_state.alpha_trust, g_state.naia_restore);
    text_puts(12, 10, buf, 7, 3);
    text_puts(20, 10, "스페이스: 타이틀로", 8, 3);
}

SceneType scene_current(void) { return current_scene; }

int scene_enter(SceneType scene) {
    current_scene = scene;
    cursor = 0;
    render_set_mode(scene == SCENE_BATTLE ? RENDER_PIXEL : RENDER_TEXT);
    return 0;
}

int scene_update(uint32_t delta_ms) {
    (void)delta_ms;
    switch (current_scene) {
        case SCENE_TITLE:  draw_title(); break;
        case SCENE_CAFE:   draw_cafe(); break;
        case SCENE_BATTLE: draw_battle(); break;
        case SCENE_ENDING: draw_ending(); break;
        case SCENE_DIALOG: dialog_draw(); break;
        case SCENE_ATELIER: draw_cafe(); break;
    }
    return 0;
}

GameKey scene_handle_key(GameKey key) {
    if (key == KEY_NONE) return KEY_NONE;

    switch (current_scene) {
    case SCENE_TITLE:
        if (key == KEY_UP)   cursor = cursor > 0 ? cursor - 1 : 1;
        if (key == KEY_DOWN) cursor = cursor < 1 ? cursor + 1 : 0;
        if (key == KEY_CONFIRM) {
            if (cursor == 0) {
                memset(&g_state, 0, sizeof(g_state));
                scene_enter(SCENE_CAFE);
                dialog_start(0);
                scene_enter(SCENE_DIALOG);
            }
        }
        break;

    case SCENE_CAFE:
        if (key == KEY_UP)   cursor = cursor > 0 ? cursor - 1 : 3;
        if (key == KEY_DOWN) cursor = cursor < 3 ? cursor + 1 : 0;
        if (key == KEY_CONFIRM) {
            g_state.cafe_explored[cursor] = 1;
            g_state.puzzles_solved++;
            g_state.naia_restore += 15;
            g_state.alpha_trust += 5;
            dialog_start(cursor + 1);
            scene_enter(SCENE_DIALOG);
        }
        if (key == KEY_MENU && g_state.chapter >= 2) {
            scene_enter(SCENE_ATELIER);
        }
        if (g_state.puzzles_solved >= 4) {
            g_state.chapter++;
            if (g_state.chapter >= 4) {
                if (g_state.naia_restore >= 60 && g_state.alpha_trust >= 20)
                    g_state.current_ending = 0;
                else if (g_state.naia_restore >= 40)
                    g_state.current_ending = 1;
                else
                    g_state.current_ending = 2;
                scene_enter(SCENE_ENDING);
            } else {
                g_state.puzzles_solved = 0;
                scene_enter(SCENE_CAFE);
            }
        }
        break;

    case SCENE_DIALOG:
        if (key == KEY_CONFIRM || key == KEY_CANCEL) {
            if (dialog_advance()) {
                scene_enter(SCENE_CAFE);
            }
        }
        break;

    case SCENE_BATTLE:
        if (key == KEY_CANCEL) scene_enter(SCENE_CAFE);
        if (key == KEY_CONFIRM) {
            g_state.naia_restore += 10;
            g_state.puzzles_solved++;
            scene_enter(SCENE_CAFE);
        }
        break;

    case SCENE_ATELIER:
        if (key == KEY_CANCEL) scene_enter(SCENE_CAFE);
        if (key == KEY_CONFIRM) {
            g_state.puzzles_solved++;
            g_state.naia_restore += 20;
            scene_enter(SCENE_BATTLE);
        }
        break;

    case SCENE_ENDING:
        if (key == KEY_CONFIRM) scene_enter(SCENE_TITLE);
        break;
    }
    return key;
}
