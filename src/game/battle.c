#include "battle.h"
#include "../engine/render/render.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOTES 32
#define LANE_COUNT 4
#define JUDGE_X 70
#define PERFECT_RANGE 2
#define GOOD_RANGE 5

static Note notes[MAX_NOTES];
static int note_count = 0;
static int score = 0;
static int gauge = 0;
static int difficulty = 1;
static int spawn_timer = 0;
static int spawn_interval = 800;
static int total_notes = 0;
static int notes_hit = 0;
static int notes_spawned = 0;

static const uint32_t jamo_chars[LANE_COUNT] = {
    0x3131, 0x3134, 0x3137, 0x3139
};

static const uint8_t lane_colors[LANE_COUNT] = { 10, 14, 12, 9 };

int battle_start(int diff) {
    difficulty = diff;
    note_count = 0;
    score = 0;
    gauge = 0;
    spawn_timer = 0;
    total_notes = 8 + diff * 4;
    notes_hit = 0;
    notes_spawned = 0;
    spawn_interval = 600 - diff * 80;
    if (spawn_interval < 200) spawn_interval = 200;
    memset(notes, 0, sizeof(notes));
    return 0;
}

static void spawn_note(void) {
    if (note_count >= MAX_NOTES) return;
    for (int i = 0; i < MAX_NOTES; i++) {
        if (!notes[i].active) {
            notes[i].lane = rand() % LANE_COUNT;
            notes[i].x = 4;
            notes[i].speed = 1 + difficulty / 2;
            notes[i].active = 1;
            note_count++;
            notes_spawned++;
            return;
        }
    }
}

int battle_update(uint32_t delta_ms) {
    spawn_timer += delta_ms;
    if (spawn_timer >= spawn_interval && notes_spawned < total_notes) {
        spawn_note();
        spawn_timer = 0;
    }

    for (int i = 0; i < MAX_NOTES; i++) {
        if (!notes[i].active) continue;
        notes[i].x += notes[i].speed;
        if (notes[i].x > JUDGE_X + GOOD_RANGE + 2) {
            notes[i].active = 0;
            note_count--;
        }
    }

    if (notes_hit >= total_notes) return 1;
    if (notes_spawned >= total_notes && note_count == 0) return 1;
    return 0;
}

static int judge_lane(int lane) {
    int best = -1;
    int best_dist = 999;
    for (int i = 0; i < MAX_NOTES; i++) {
        if (!notes[i].active || notes[i].lane != lane) continue;
        int dist = abs(notes[i].x - JUDGE_X);
        if (dist < best_dist) {
            best_dist = dist;
            best = i;
        }
    }
    if (best < 0) return 0;
    if (best_dist <= PERFECT_RANGE) {
        notes[best].active = 0;
        note_count--;
        notes_hit++;
        score += 100;
        gauge += 3;
        return 2;
    }
    if (best_dist <= GOOD_RANGE) {
        notes[best].active = 0;
        note_count--;
        notes_hit++;
        score += 50;
        gauge += 1;
        return 1;
    }
    return 0;
}

int battle_handle_key(GameKey key) {
    int lane = -1;
    switch (key) {
        case KEY_UP:    lane = 0; break;
        case KEY_DOWN:  lane = 1; break;
        case KEY_LEFT:  lane = 2; break;
        case KEY_RIGHT: lane = 3; break;
        case KEY_CANCEL: return -1;
        default: return 0;
    }
    if (lane >= 0) judge_lane(lane);
    return 0;
}

void battle_draw(void) {
    render_clear();
    render_set_mode(RENDER_PIXEL);
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 0, 1);

    text_puts(0, 2, "리듬 정렬", 15, 1);

    char buf[40];
    snprintf(buf, sizeof(buf), "점수:%d  게이지:%d%%", score, gauge > 100 ? 100 : gauge);
    text_puts(0, 20, buf, 14, 1);

    for (int lane = 0; lane < LANE_COUNT; lane++) {
        int y = 4 + lane * 5;
        for (int x = 4; x <= JUDGE_X + 5; x++) {
            text_putc(y, x, '-', 4, 1);
        }
        text_putc(y, 3, jamo_chars[lane], lane_colors[lane], 1);
        text_putc(y, JUDGE_X, '>', 15, 1);
    }

    for (int i = 0; i < MAX_NOTES; i++) {
        if (!notes[i].active) continue;
        int y = 4 + notes[i].lane * 5;
        if (notes[i].x >= 4 && notes[i].x < SCREEN_COLS) {
            text_putc(y, notes[i].x, 0x2588, lane_colors[notes[i].lane], 1);
        }
    }

    int gauge_len = gauge / 5;
    if (gauge_len > 20) gauge_len = 20;
    text_fill(22, 5, 20, 1, ' ', 8, 8);
    text_fill(22, 5, gauge_len, 1, 0x2588, 10, 2);

    text_puts(23, 5, "방향키=레인  Q=나가기", 8, 1);
}
