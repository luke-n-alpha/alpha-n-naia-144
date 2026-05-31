#include "save.h"
#include <stdio.h>
#include <string.h>

#define SAVE_PATH     "save_%d.dat"
#define MAX_SAVE_SIZE 1024

static int slot_ok(int slot) { return slot >= 0 && slot <= 2; }

/* state_id 문자열(NUL 종단)을 슬롯에 저장. 매 write 는 슬롯을 완전 대체. 성공 OK. */
int save_write(int slot, const char *state_id) {
    if (!slot_ok(slot) || !state_id) return ERR_ARG;
    size_t len = strlen(state_id);
    if (len > MAX_SAVE_SIZE) return ERR_FULL;
    char path[32];
    snprintf(path, sizeof(path), SAVE_PATH, slot);
    FILE *f = fopen(path, "wb");          /* "wb" = 기존 내용 절단(overwrite) */
    if (!f) return ERR_IO;
    size_t wrote = fwrite(state_id, 1, len, f);
    int rc = (fclose(f) != 0 || wrote != len) ? ERR_IO : OK;
    return rc;
}

/* 슬롯의 state_id 를 buf 로 복원(NUL 종단). buf 가 작으면 ERR_ARG(무손실 — silent truncation 금지).
 * 부재 슬롯은 ERR_IO. interfaces.md 는 truncation 동작을 명세 안 함 → 무손실을 conformant 기본으로 채택(후속: 계약 명시). */
int save_read(int slot, char *state_id, int buf_size) {
    if (!slot_ok(slot) || !state_id || buf_size <= 0) return ERR_ARG;
    char path[32];
    snprintf(path, sizeof(path), SAVE_PATH, slot);
    FILE *f = fopen(path, "rb");
    if (!f) return ERR_IO;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return ERR_IO; }
    long sz = ftell(f);
    if (sz < 0 || fseek(f, 0, SEEK_SET) != 0) { fclose(f); return ERR_IO; }
    if (sz + 1 > buf_size) { fclose(f); return ERR_ARG; }   /* 다 못 담으면 거부 */
    size_t rd = fread(state_id, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) return ERR_IO;
    state_id[sz] = '\0';
    return OK;
}

int save_erase(int slot) {
    if (!slot_ok(slot)) return ERR_ARG;
    char path[32];
    snprintf(path, sizeof(path), SAVE_PATH, slot);
    return (remove(path) != 0) ? ERR_IO : OK;
}
