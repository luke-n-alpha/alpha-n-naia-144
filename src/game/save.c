#include "save.h"
#include <stdio.h>
#include <string.h>

#define SAVE_PATH "save_%d.dat"
#define MAX_SAVE_SIZE 1024

int save_write(int slot, const char *data, int len) {
    if (slot < 0 || slot > 2 || !data) return -1;
    if (len > MAX_SAVE_SIZE) return -2;
    char path[32];
    snprintf(path, sizeof(path), SAVE_PATH, slot);
    FILE *f = fopen(path, "wb");
    if (!f) return -3;
    fwrite(&len, sizeof(int), 1, f);
    int wrote = fwrite(data, 1, len, f);
    fclose(f);
    if (wrote != len) return -4;
    return 0;
}

int save_read(int slot, char *data, int buf_size) {
    if (slot < 0 || slot > 2 || !data) return -1;
    char path[32];
    snprintf(path, sizeof(path), SAVE_PATH, slot);
    FILE *f = fopen(path, "rb");
    if (!f) return -2;
    int len = 0;
    fread(&len, sizeof(int), 1, f);
    if (len <= 0 || len > buf_size) { fclose(f); return -3; }
    int read_n = fread(data, 1, len, f);
    fclose(f);
    if (read_n != len) return -4;
    if (len < buf_size) data[len] = '\0';
    return len;
}

int save_erase(int slot) {
    if (slot < 0 || slot > 2) return -1;
    char path[32];
    snprintf(path, sizeof(path), SAVE_PATH, slot);
    return remove(path);
}
