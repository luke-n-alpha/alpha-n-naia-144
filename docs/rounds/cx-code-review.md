# CX Code Review

검토 범위:
- `src/`의 모든 `.c`, `.h`
- `tests/test_all.c`
- 요청 경로 `docs/game-design.md`는 저장소에 없어서 실제 존재하는 `docs/contracts/game-design.md`를 검토함

검증 메모:
- `gcc -std=c99 -Wall -Wextra -I src ... src/main.c ...` 빌드는 실패했다. `src/main.c:23-45`의 POSIX 시간 API 사용이 현재 C99 계약과 바로 맞지 않는다.
- `gcc -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -I src tests/test_all.c ...` 및 `/tmp/alpha_tests`는 통과했지만, 테스트 자체에 누락과 허위 통과가 있다.

## File-by-file Findings

### `src/main.c`
- `HIGH` `src/main.c:23-45` `struct timespec`, `clock_gettime`, `nanosleep`를 사용하지만 `_POSIX_C_SOURCE` 같은 feature macro 없이 C99 빌드를 깨뜨린다. `docs/contracts/tech-stack.md:12-16`의 C99 계약 기준으로는 현재 엔트리포인트가 바로 컴파일되지 않는다.
- `HIGH` `src/main.c:11` `render_init()`를 인자 없이 호출한다. 인터페이스 계약은 `render_init(RenderMode mode)`다 (`docs/contracts/interfaces.md:59`).

### `src/engine/font/font.h`
- `LOW` `src/engine/font/font.h:15-18` `GlyphEntry.codepoint`가 `uint16_t`다. 계약은 `uint32_t` Unicode codepoint 입력 기준이다 (`docs/contracts/interfaces.md:90-95`). 현재 struct는 미사용이지만 타입 정의부터 계약과 어긋난다.

### `src/engine/font/font.c`
- `HIGH` `src/engine/font/font.c:4-13` `font_get_glyph()`가 ASCII만 매핑하고 나머지는 모두 fallback으로 보낸다. 계약상 한글 자모와 기호 직접 인덱싱/NFD 기반 처리가 있어야 한다 (`docs/contracts/interfaces.md:90-95`).
- `HIGH` `src/engine/font/font.c:10-13` 박스드로잉과 자모가 모두 공백 글리프로 떨어져 UI와 한글 타일 렌더가 사실상 미구현 상태다.

### `src/engine/font/glyph_data.c`
- `HIGH` `src/engine/font/glyph_data.c:3-18` `GLYPH_TOTAL`은 203개인데 실제 초기화는 ASCII 95개뿐이다. 자모 68개와 UI/박스드로잉 40개가 전부 비어 있다.
- `HIGH` `src/engine/font/glyph_data.c:3-18` 각 `GlyphBitmap`이 실제 8x16 비트맵이 아니라 첫 1-2바이트만 문자 코드로 채워진 placeholder다. `docs/contracts/tile-spec.md:12-16`의 글리프 포맷을 충족하지 못한다.
- `MEDIUM` `src/engine/font/glyph_data.c:20-28` `JAMO_BRIGHTNESS`가 수동 상수 배열이다. 타일 계약은 실측 자동 생성 테이블을 요구한다 (`docs/contracts/tile-spec.md:52-61`).

### `src/engine/input/input.h`
- `MEDIUM` `src/engine/input/input.h:4-9` `KEY_NONE`를 `KEY_MENU`와 `KEY_COUNT` 사이에 삽입해서 enum 값과 개수가 계약과 달라졌다. 계약상 키맵은 `KEY_UP..KEY_MENU, KEY_COUNT`다 (`docs/contracts/interfaces.md:163-171`).

### `src/engine/input/input.c`
- `MEDIUM` `src/engine/input/input.c:10-18` `tcgetattr`/`tcsetattr` 실패를 무시하고 항상 0을 반환한다. tty가 아니거나 권한이 없을 때도 초기화 성공으로 오인한다.
- `LOW` `src/engine/input/input.c:49-50` `input_shutdown()`도 초기화 성공 여부나 `tcsetattr` 실패를 확인하지 않는다.

### `src/engine/render/render.h`
- `HIGH` `src/engine/render/render.h:20` 선언이 `int render_init(void);`다. 계약은 `int render_init(RenderMode mode);`다 (`docs/contracts/interfaces.md:59`).
- `MEDIUM` `src/engine/render/render.h:34-36` 팔레트가 `PALETTE_R/G/B` 3배열로 노출된다. 계약은 `Color PALETTE[16]`다 (`docs/contracts/interfaces.md:101-108`).

### `src/engine/render/render.c`
- `CRITICAL` `src/engine/render/render.c:146-158` pixel backend가 계약상 `640x400`급 픽셀 버퍼여야 하는데, 실제로는 `(x/8, y/16)` 셀 하나를 `0x2588` 문자로 칠할 뿐이다. 픽셀 단위 상태가 없어서 같은 셀 내부의 서로 다른 픽셀을 표현할 수 없다.
- `HIGH` `src/engine/render/render.c:123-136` `text_puts()`는 UTF-8 codepoint를 1셀씩 그대로 찍는다. 계약상 한글은 자모 분해 후 3개 타일 배치가 필요하다 (`docs/contracts/interfaces.md:93-95`).
- `MEDIUM` `src/engine/render/render.c:47-50` `render_set_mode()`가 유효성 검증 없이 모드만 바꾸고, backend 전환에 필요한 상태 준비가 없다.
- `LOW` `src/engine/render/render.c:146-158` `color_idx` 상한 검증이 없어 16색 계약 밖 값이 그대로 들어갈 수 있다.

### `src/game/battle.h`
- 파일 자체에서는 추가 발견사항 없음.

### `src/game/battle.c`
- `MEDIUM` `src/game/battle.c:123-156` `RENDER_PIXEL` 모드라고 해놓고 실제 그리기는 전부 `text_putc`/`text_fill`로 한다. 전투가 계약상의 pixel backend 소비자가 아니다.
- `LOW` `src/game/battle.c:17,95,103,131,151-154` `gauge`는 누적만 되고 내부 값은 clamp되지 않는다. 표시만 100으로 막아서 상태와 UI가 분리된다.

### `src/game/dialog.h`
- `HIGH` `src/game/dialog.h:4-6` 계약에 있는 시나리오 API (`scenario_load`, `scenario_next`, `scenario_choose`, `scenario_get_state_id` 등)가 전혀 없고, 비계약 `dialog_*` API만 노출한다 (`docs/contracts/interfaces.md:132-147`).

### `src/game/dialog.c`
- `HIGH` `src/game/dialog.c:5-270` 전체 모듈이 하드코딩된 대사 테이블만 제공한다. 계약의 `DialogLine { speaker_id, emotion_id, line }` 구조와 scenario 로드/선택/상태 저장 인터페이스가 전부 미구현이다.
- `LOW` `src/game/dialog.c:108` `"농춮되어"` 오탈자가 있다. 콘텐츠 검수 이전에도 노출될 문자열이다.

### `src/game/save.h`
- `HIGH` `src/game/save.h:4-6` 저장 계약은 `save_write(int slot, const char *state_id)` / `save_read(...)`인데, 현재 헤더는 길이 인자를 받는 바이너리 blob API다 (`docs/contracts/interfaces.md:151-157`).

### `src/game/save.c`
- `HIGH` `src/game/save.c:27-32` `save_read()`가 읽은 바이트 뒤에 NUL terminator를 쓰지 않는다. 지금 테스트는 버퍼를 0으로 초기화해서 우연히 통과하지만, 일반 문자열 소비자는 쉽게 over-read 한다.
- `MEDIUM` `src/game/save.c:15-16,28-30` `fwrite`/`fread` 반환값을 확인하지 않는다. 잘린 파일이나 부분 쓰기를 정상 저장으로 오인한다.
- `MEDIUM` `src/game/save.c:15` 길이 프리픽스를 native `int`로 그대로 저장한다. 슬롯당 약 1KB의 경량 세이브 계약은 맞추려 하지만, 포맷 안정성과 호환성은 없다.

### `src/game/scene.h`
- `HIGH` `src/game/scene.h:29-33` `scene_exit()`가 선언되어 있지만 정의가 없다. 계약에도 있는 필수 인터페이스다 (`docs/contracts/interfaces.md:124-127`).
- `MEDIUM` `src/game/scene.h:32` `scene_handle_key()`가 `GameKey`를 반환한다. 계약은 `int scene_handle_key(int keycode)`다 (`docs/contracts/interfaces.md:124-127`).

### `src/game/scene.c`
- `HIGH` `src/game/scene.c:194-224` 카페 조사만 4번 하면 `puzzles_solved >= 4`로 즉시 챕터가 넘어간다. 아틀리에/배틀을 거치지 않고도 진행과 엔딩 도달이 가능해서 `docs/contracts/game-design.md:14,39-40`의 Pixel mode 전투 핵심 메커니즘이 사실상 우회된다.
- `MEDIUM` `src/game/scene.c:76,191-204` UI는 카페에서 `Q:back`을 안내하지만 `SCENE_CAFE` 분기에서 `KEY_CANCEL`을 처리하지 않는다.
- `MEDIUM` `src/game/scene.c:101-112` 엔딩 설명 문자열에 `\n`이 포함되어 있는데, 현재 `text_puts()`는 줄바꿈을 해석하지 않는다. 엔딩 텍스트 레이아웃이 깨진다.
- `LOW` `src/game/scene.c:13` `battle_done`가 설정만 되고 사용되지 않는다.

### `src/game/world.h`
- `MEDIUM` `src/game/world.h:4-5` 월드 API가 존재하지만 현재 게임 루프와 인터페이스 계약 어디에도 연결되지 않는다. 모듈 경계만 있고 실사용 경로가 없다.

### `src/game/world.c`
- `MEDIUM` `src/game/world.c:3-4` `world_init()`/`world_update()`가 완전 stub이다. 월드/맵 구조가 산출물 목록에 포함돼 있는데 (`docs/00-charter/process-and-deliverables.md:77`) 구현이 비어 있다.

### `tests/test_all.c`
- `HIGH` `tests/test_all.c:147-151` `if (done || 1) PASS();` 때문에 전투 완료 여부와 무관하게 항상 PASS다. 가장 중요한 전투 종료 조건 검증이 무효다.
- `HIGH` `tests/test_all.c:174-179,187-190` 통합 테스트가 전투 장면에서 입력 몇 개 후 `KEY_CANCEL`로 빠져나오는 흐름도 허용한다. 현재 “전체 플레이스루 PASS”는 배틀을 건너뛰어도 성립한다.
- `MEDIUM` `tests/test_all.c:108-112` dialog 검증이 `0..19`만 돈다. 실제 대화 테이블은 24개다 (`src/game/dialog.c:217-224`).
- `MEDIUM` `tests/test_all.c:121-125` 저장 roundtrip 테스트가 `char buf[256] = {0};`로 버퍼를 미리 0으로 채워서 `save_read()`의 NUL 미종결 버그를 숨긴다.
- `MEDIUM` `tests/test_all.c:193-227` 테스트 바이너리는 `src/main.c`를 포함하지 않아, 실제 엔트리포인트의 C99 빌드 실패를 잡지 못한다.

### `docs/contracts/game-design.md`
- `LOW` `docs/contracts/game-design.md:1` 사용자 요청의 경로 `docs/game-design.md`는 현재 저장소에 없다. 실제 계약 문서는 이 파일 경로에만 존재한다.
- `MEDIUM` `docs/contracts/game-design.md:14-19,39-40` 문서가 요구하는 “Pixel mode 전투/이펙트”, “3슬롯 세이브”, “방향키+확인/취소/메뉴” 중 전투와 저장은 코드가 부분 구현 또는 계약 이탈 상태다. 문서 자체 문제라기보다 현재 구현과의 괴리가 크다.

## Files With No Additional Findings

- `src/game/battle.h`

## Priority Summary

1. `CRITICAL`: `render.c`의 pixel backend 미구현.
2. `HIGH`: `font.c`/`glyph_data.c`의 한글·UI 글리프 미구현.
3. `HIGH`: `scene.c`의 챕터 진행이 전투를 우회함.
4. `HIGH`: `save.c` 문자열 저장/복구 안전성 부족.
5. `HIGH`: `tests/test_all.c` 전투/통합 테스트가 허위 통과를 만든다.
