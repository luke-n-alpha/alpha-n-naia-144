# CX Code Review #2

검토 범위:
- `src/` 전체
- `tests/test_all.c`
- 기준 문서: `docs/rounds/cx-code-review.md`

검증 메모:
- `gcc -std=c99 -Wall -Wextra -I src ... src/main.c ...`는 여전히 실패했다. `src/main.c:23-45`의 POSIX 시간 API가 feature macro 없이 남아 있다.
- `gcc -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -I src tests/test_all.c ...` 및 `/tmp/alpha_tests`는 통과했다.

## Original CRITICAL/HIGH Status

### 1. `HIGH` `src/main.c` POSIX 시간 API로 C99 빌드가 깨짐
**Status: REMAINING**

`src/main.c:23-45`는 아직 `struct timespec`, `clock_gettime`, `CLOCK_MONOTONIC`, `nanosleep`를 직접 사용한다. 현재도 `_POSIX_C_SOURCE` 없이 순수 `-std=c99` 빌드 시 컴파일이 깨진다.

### 2. `HIGH` `src/main.c`가 `render_init()`를 인자 없이 호출
**Status: FIXED**

현재 `src/main.c:11`은 `render_init(RENDER_TEXT)`로 호출한다. `src/engine/render/render.h:20` 선언과도 일치한다.

### 3. `HIGH` `src/engine/font/font.c`가 ASCII만 매핑하고 자모/기호를 fallback 처리
**Status: REMAINING**

`src/engine/font/font.c:4-18`은 예전보다 범위를 넓혔지만, 여전히 조합형 한글 codepoint를 분해하지 못한다. 예를 들어 `text_puts()`가 넘기는 일반 한글 음절은 `font_get_glyph()`에서 직접 처리되지 않는다. 계약의 “Unicode codepoint 입력 + 내부 NFD 분해” 기준은 아직 충족하지 못한다.

### 4. `HIGH` `src/engine/font/font.c`에서 박스드로잉/자모가 공백 글리프로 떨어짐
**Status: REMAINING**

`font.c`의 인덱싱 범위가 생겼어도 실제 자산이 따라오지 않는다. `src/engine/font/glyph_data.c:3-103`의 `FONT_GLYPHS`는 명시 초기화가 95개뿐이고, 나머지 자모/박스드로잉 슬롯은 0으로 남아 공백 비트맵이 된다.

### 5. `HIGH` `src/engine/font/glyph_data.c`에서 `GLYPH_TOTAL` 203개 중 실제 초기화는 ASCII 95개뿐
**Status: REMAINING**

현재도 동일하다. `GLYPH_TOTAL`은 `src/engine/font/font.h:26`에서 203개인데, `src/engine/font/glyph_data.c`의 명시 초기화 엔트리는 95개뿐이다. 나머지 108개 슬롯은 zero-init 상태다.

### 6. `HIGH` `src/engine/font/glyph_data.c`의 글리프가 8x16 실제 비트맵이 아닌 placeholder
**Status: REMAINING**

ASCII 일부는 간단한 비트패턴이 들어갔지만, 자모/UI/박스드로잉 쪽은 여전히 실질적으로 비어 있다. `src/engine/font/glyph_data.c:3-103` 전체가 계약의 “68 자모 + 40 UI/박스드로잉 8x16 비트맵”을 채우지 못한다.

### 7. `HIGH` `src/engine/render/render.h` 선언이 `int render_init(void);`라 계약과 불일치
**Status: FIXED**

현재 `src/engine/render/render.h:20`은 `int render_init(RenderMode mode);`로 수정됐다.

### 8. `CRITICAL` `src/engine/render/render.c` pixel backend가 셀 하나를 `█` 문자로만 칠해 픽셀 상태가 없음
**Status: FIXED**

현재 `src/engine/render/render.c:39-45`에 `PixelCell`과 `pixel_buf[25][80]`가 있고, `pixel_set()`이 `sub[16][8]` 단위로 픽셀 상태를 기록한다 (`src/engine/render/render.c:186-195`). 같은 셀 내부의 서로 다른 픽셀 값을 따로 저장할 수 있게 됐다는 점에서 최초 `CRITICAL` 근거는 해소됐다.

### 9. `HIGH` `src/engine/render/render.c` `text_puts()`가 UTF-8 codepoint를 1셀씩만 찍고 한글 자모 분해를 안 함
**Status: REMAINING**

`src/engine/render/render.c:163-176`은 여전히 codepoint 하나를 셀 하나에 바로 넣는다. 조합형 한글을 초성/중성/종성 3타일로 배치하는 로직은 없다.

### 10. `HIGH` `src/game/dialog.h`에 계약상의 scenario API가 없음
**Status: REMAINING**

현재 헤더는 `src/game/dialog.h:4-6`의 `dialog_start`, `dialog_advance`, `dialog_draw`만 제공한다. `scenario_load`, `scenario_next`, `scenario_choose`, `scenario_get_state_id` 등 계약 API는 여전히 없다.

### 11. `HIGH` `src/game/dialog.c`가 하드코딩 대사 테이블만 제공하고 scenario/load-choice-state 인터페이스가 없음
**Status: REMAINING**

`src/game/dialog.c:12-224`는 여전히 정적 대사 배열 중심 구현이다. 계약의 `DialogLine` 기반 scenario 로드/선택/상태 저장 인터페이스는 구현되지 않았다.

### 12. `HIGH` `src/game/save.h` 저장 API가 계약과 다름
**Status: REMAINING**

현재도 `src/game/save.h:4-6`은 `save_write(int slot, const char *state_json, int len)` 형태다. 계약의 `save_write(int slot, const char *state_id)` / `save_read(...)`와 맞지 않는다.

### 13. `HIGH` `src/game/save.c` `save_read()`가 NUL terminator를 보장하지 않음
**Status: REMAINING**

`src/game/save.c:34`에서 `len < buf_size`일 때만 종단 NUL을 쓴다. 즉 저장 길이가 `buf_size`와 같으면 여전히 미종결 문자열을 반환할 수 있다. 최초 지적은 완전히 해소되지 않았다.

### 14. `HIGH` `src/game/scene.h`에 선언된 `scene_exit()` 정의가 없음
**Status: FIXED**

현재 `src/game/scene.c:123-126`에 `scene_exit()` 정의가 존재한다.

### 15. `HIGH` `src/game/scene.c` 카페 조사 4번만으로 전투 없이 챕터/엔딩 진행 가능
**Status: FIXED**

현재 `src/game/scene.c:165-185`의 `advance_chapter()`는 `chapter >= 2`에서 `battle_done == 0`이면 먼저 `SCENE_BATTLE`로 강제 이동한다. 최소한 엔딩까지 가는 핵심 진행에서 전투는 이제 우회되지 않는다.

### 16. `HIGH` `tests/test_all.c` 전투 테스트가 `if (done || 1)`로 항상 PASS
**Status: FIXED**

현재 `tests/test_all.c:147-156`은 `done`이 실제로 1이 되어야만 PASS한다. 허위 통과 조건은 제거됐다.

### 17. `HIGH` `tests/test_all.c` 통합 테스트가 배틀을 건너뛰어도 PASS
**Status: REMAINING**

예전의 `KEY_CANCEL` 경로는 사라졌지만, 테스트는 여전히 엄격하지 않다. `tests/test_all.c:203-207`은 엔딩 도달 실패 후에도 `scenes_visited > 6`이면 PASS 처리한다. 그래서 전체 플레이스루가 실제 완료되지 않아도 허위 통과가 남아 있다.

## Summary

- FIXED: 6
- REMAINING: 11

핵심 잔존 이슈는 그대로 `font/glyph 자산 미구현`, `한글 자모 분해 렌더 미구현`, `scenario/save 인터페이스 계약 불일치`, `main.c`의 C99 빌드 파손, `통합 테스트 false positive`다.
