#ifndef SAVE_H
#define SAVE_H

/* 공통 에러 코드 — interfaces.md §공통. (설계엔 있고 코드엔 부재였던 드리프트, 여기서 도입.)
 * NOTE: §공통 코드의 정본은 공유 헤더(예: src/common.h)여야 함 — 별도 후속(broader 드리프트). */
#ifndef OK
#define OK        0
#define ERR_ARG  -1
#define ERR_IO   -2
#define ERR_MEM  -3
#define ERR_FULL -4
#endif

/* Save System — interfaces.md §6 (canonical). state_id = 논리적 상태 문자열(NUL 종단). */
int save_write(int slot, const char *state_id);
int save_read(int slot, char *state_id, int buf_size);
int save_erase(int slot);

#endif
