#include "scene.h"
#include "../engine/render/render.h"
#include "dialog.h"
#include "battle.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

GameState g_state = {0};
static SceneType current_scene = SCENE_TITLE;
static int cursor = 0;
static int battle_done = 0;

static const char *season_name(int ch) {
    switch (ch) {
        case 0: return "봄";
        case 1: return "여름";
        case 2: return "가을";
        case 3: return "겨울";
        default: return "?";
    }
}

static uint8_t season_fg(int ch) {
    static const uint8_t fgs[] = {10, 14, 12, 15};
    return (ch >= 0 && ch < 4) ? fgs[ch] : 7;
}

static const char *cafe_spots[] = {"카운터", "피아노", "소파", "창가"};

static void draw_title(void) {
    render_clear();
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 8, 0);
    for (int i = 0; i < 60; i++) {
        uint8_t br = (i * 7 + 3) % 16;
        text_putc(2, 10 + i, 0x2588, br, 0);
    }
    text_puts(5, 18, "자모등 아래의 나이아", 15, 0);
    text_puts(6, 18, "Naia Beneath the Jamo Lights", 8, 0);
    text_puts(10, 20, "> 새 게임", cursor == 0 ? 15 : 7, 0);
    text_puts(11, 20, "  이어하기", cursor == 1 ? 15 : 7, 0);
    text_puts(15, 5, "cafelua x naia  |  1.44MB Disk Game", 3, 0);
    text_puts(22, 5, "Arrow:move  Space:select  Q:quit", 8, 0);
}

static void draw_cafe(void) {
    render_clear();
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 0, 2);
    text_puts(0, 0, "cafelua - ", season_fg(g_state.chapter), 2);
    text_puts(0, 10, season_name(g_state.chapter), season_fg(g_state.chapter), 2);
    text_puts(0, 13, " - 마감 후", 7, 2);

    for (int i = 0; i < 4; i++) {
        int y = 4 + i * 3;
        uint8_t fg = (cursor == i) ? 15 : 7;
        if (g_state.cafe_explored[i]) {
            text_puts(y, 5, "[v] ", 10, 2);
        } else if (cursor == i) {
            text_puts(y, 5, ">   ", 14, 2);
        } else {
            text_puts(y, 5, "    ", 7, 2);
        }
        text_puts(y, 9, cafe_spots[i], fg, 2);
    }

    if (g_state.chapter >= 2)
        text_puts(13, 55, "[M] 2F", 12, 2);

    char status[80];
    snprintf(status, sizeof(status),
             "Ch.%d %s | 복원:%d%% | 신뢰:%d | 퍼즐:%d/4",
             g_state.chapter + 1, season_name(g_state.chapter),
             g_state.naia_restore, g_state.alpha_trust, g_state.puzzles_solved);
    text_puts(20, 2, status, 8, 0);
    text_puts(23, 2, "Arrow:move  Space:조사  M:2F  Q:back", 8, 0);
}

static void draw_atelier(void) {
    render_clear();
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 0, 5);
    text_puts(0, 2, "2F 아틀리에 - 기억 복원 공간", 15, 5);
    text_puts(4, 5, "벽에 걸린 그림들이 희미하게 빛나고 있다.", 14, 5);
    text_puts(6, 5, "자모 타일의 밝기가 불안정하게 떨리고 있다.", 14, 5);

    if (g_state.chapter >= 3) {
        text_puts(10, 5, "naia의 흔적이 진하게 남아있다.", 13, 5);
        text_puts(12, 5, "리듬 정렬을 시작하려면 스페이스.", 15, 5);
    } else {
        text_puts(10, 5, "아직은 조사할 단서가 부족하다.", 8, 5);
    }

    text_puts(23, 5, "Space:시작  Q:돌아가기", 8, 5);
}

static void draw_ending(void) {
    render_clear();
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 0, 3);

    const char *titles[] = {"일상 회복", "기억 공명", "미해결 새벽"};
    const char *descs[] = {
        "카페에 아침이 돌아왔다.\nAlpha가 창문을 연다.\nnaia가 소파 위에서 하품한다.\n모든 것이 제자리다.",
        "naia가 빛의 패턴으로 마지막 메시지를 남겼다.\n\"다시 만나자\" \n밝기가 안정되고, 카페에 온기가 돌아온다.",
        "밤이 끝나지 않았다.\n자모의 밝기는 여전히 불안정하다.\n하지만 luke는 내일 다시 카페를 연다."
    };

    int e = g_state.current_ending;
    if (e < 0 || e > 2) e = 2;

    text_puts(3, 25, "~ 엔 딩 ~", 15, 3);
    text_puts(5, 25, titles[e], 14, 3);
    text_puts(9, 10, descs[e], 15, 3);

    char buf[80];
    snprintf(buf, sizeof(buf), "Alpha 신뢰도: %d | naia 복원률: %d%%",
             g_state.alpha_trust, g_state.naia_restore);
    text_puts(18, 10, buf, 7, 3);
    text_puts(22, 25, "Space: 타이틀", 8, 3);
}

SceneType scene_current(void) { return current_scene; }

int scene_exit(void) {
    render_shutdown();
    return 0;
}

int scene_enter(SceneType scene) {
    current_scene = scene;
    cursor = 0;
    battle_done = 0;
    render_set_mode(scene == SCENE_BATTLE ? RENDER_PIXEL : RENDER_TEXT);
    if (scene == SCENE_BATTLE) {
        battle_start(g_state.chapter + 1);
    }
    return 0;
}

int scene_update(uint32_t delta_ms) {
    switch (current_scene) {
        case SCENE_TITLE:  draw_title(); break;
        case SCENE_CAFE:   draw_cafe(); break;
        case SCENE_ATELIER: draw_atelier(); break;
        case SCENE_BATTLE:
            if (battle_update(delta_ms)) {
                g_state.naia_restore += 10 + g_state.chapter * 5;
                g_state.puzzles_solved++;
                battle_done = 1;
                dialog_start(10 + g_state.chapter);
                scene_enter(SCENE_DIALOG);
                return 0;
            }
            battle_draw();
            break;
        case SCENE_ENDING: draw_ending(); break;
        case SCENE_DIALOG: dialog_draw(); break;
    }
    return 0;
}

static int battle_required(void) {
    return g_state.chapter >= 2;
}

static void advance_chapter(void) {
    if (battle_required() && !battle_done) {
        scene_enter(SCENE_BATTLE);
        return;
    }
    g_state.chapter++;
    g_state.puzzles_solved = 0;
    battle_done = 0;
    memset(g_state.cafe_explored, 0, sizeof(g_state.cafe_explored));
    if (g_state.chapter >= 4) {
        if (g_state.naia_restore >= 60 && g_state.alpha_trust >= 20)
            g_state.current_ending = 0;
        else if (g_state.naia_restore >= 40)
            g_state.current_ending = 1;
        else
            g_state.current_ending = 2;
        scene_enter(SCENE_ENDING);
    } else {
        dialog_start(5 + (g_state.chapter - 1) * 5);
        scene_enter(SCENE_DIALOG);
    }
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
                srand((unsigned)time(NULL));
                dialog_start(0);
                scene_enter(SCENE_DIALOG);
            }
        }
        break;

    case SCENE_CAFE:
        if (key == KEY_UP)   cursor = cursor > 0 ? cursor - 1 : 3;
        if (key == KEY_DOWN) cursor = cursor < 3 ? cursor + 1 : 0;
        if (key == KEY_CONFIRM && !g_state.cafe_explored[cursor]) {
            g_state.cafe_explored[cursor] = 1;
            g_state.puzzles_solved++;
            g_state.naia_restore += 5 + g_state.chapter * 3;
            g_state.alpha_trust += 3;
            dialog_start(1 + cursor + g_state.chapter * 4);
            scene_enter(SCENE_DIALOG);
        }
        if (key == KEY_MENU && g_state.chapter >= 2)
            scene_enter(SCENE_ATELIER);
        break;

    case SCENE_ATELIER:
        if (key == KEY_CANCEL) scene_enter(SCENE_CAFE);
        if (key == KEY_CONFIRM && g_state.chapter >= 3)
            scene_enter(SCENE_BATTLE);
        break;

    case SCENE_BATTLE: {
        int r = battle_handle_key(key);
        if (r < 0) scene_enter(SCENE_CAFE);
        break;
    }

    case SCENE_DIALOG:
        if (key == KEY_CONFIRM || key == KEY_CANCEL) {
            if (dialog_advance()) {
                if (g_state.puzzles_solved >= 4)
                    advance_chapter();
                else
                    scene_enter(SCENE_CAFE);
            }
        }
        break;

    case SCENE_ENDING:
        if (key == KEY_CONFIRM) scene_enter(SCENE_TITLE);
        break;
    }
    return key;
}
