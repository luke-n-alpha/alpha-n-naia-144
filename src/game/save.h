#ifndef SAVE_H
#define SAVE_H

int save_write(int slot, const char *state_json, int len);
int save_read(int slot, char *state_json, int buf_size);
int save_erase(int slot);

#endif
