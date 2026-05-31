/*
 * 설계-first 테스트 — save 시스템 (interfaces.md §6 canonical: 2-arg state_id, §공통 OK/ERR).
 * 드리프트 해소 후 강화판. codex judge 결손 반영(overwrite/empty/null/무손실/return규약).
 * 단언은 설계 의도(round-trip·무손실·슬롯경계·슬롯독립)에서 도출, *내용(buffer) 검증*으로 teeth 확보.
 * 버킷: save-conform-interfaces. generator(impl)=claude, judge=codex(external).
 */
#include "save.h"
#include <string.h>
#include <stdio.h>

static int fails = 0;
#define CHECK(c) do { if (!(c)) { printf("FAIL: %s (line %d)\n", #c, __LINE__); fails++; } } while (0)

int main(void) {
    char buf[64];

    /* round-trip: 쓴 문자열 그대로 (내용 검증 = teeth) */
    CHECK(save_write(0, "ch:2,nr:45") == OK);
    CHECK(save_read(0, buf, sizeof buf) == OK);
    CHECK(strcmp(buf, "ch:2,nr:45") == 0);

    /* 빈 문자열도 round-trip */
    CHECK(save_write(0, "") == OK);
    CHECK(save_read(0, buf, sizeof buf) == OK);
    CHECK(strcmp(buf, "") == 0);

    /* overwrite: 두 번째가 완전 대체, 짧게 덮어도 잔재 없음 */
    CHECK(save_write(0, "FIRST") == OK);
    CHECK(save_write(0, "SECOND-LONGER") == OK);
    CHECK(save_read(0, buf, sizeof buf) == OK);
    CHECK(strcmp(buf, "SECOND-LONGER") == 0);
    CHECK(save_write(0, "X") == OK);
    CHECK(save_read(0, buf, sizeof buf) == OK);
    CHECK(strcmp(buf, "X") == 0);

    /* erase 후 read 는 부재(에러) */
    CHECK(save_erase(0) == OK);
    CHECK(save_read(0, buf, sizeof buf) < 0);

    /* 슬롯 경계 (설계: 0..2) — 에러코드까지 규약대로 */
    CHECK(save_write(-1, "x") == ERR_ARG);
    CHECK(save_write(3, "x") == ERR_ARG);
    CHECK(save_read(-1, buf, sizeof buf) == ERR_ARG);

    /* NULL 인자 거부 */
    CHECK(save_write(0, NULL) == ERR_ARG);
    CHECK(save_read(0, NULL, sizeof buf) == ERR_ARG);
    save_erase(0);

    /* 무손실: buf 가 작으면 truncate 말고 거부 */
    CHECK(save_write(0, "ABCDEFGHIJ") == OK);   /* 10 + NUL = 11 */
    { char small[8]; CHECK(save_read(0, small, sizeof small) < 0); }
    save_erase(0);

    /* 슬롯 독립성 */
    CHECK(save_write(1, "ONE") == OK);
    CHECK(save_write(2, "TWO") == OK);
    CHECK(save_read(1, buf, sizeof buf) == OK && strcmp(buf, "ONE") == 0);
    CHECK(save_read(2, buf, sizeof buf) == OK && strcmp(buf, "TWO") == 0);
    save_erase(1);
    save_erase(2);

    if (fails) { printf("%d CHECK failed\n", fails); return 1; }
    printf("save conform-interfaces OK\n");
    return 0;
}
