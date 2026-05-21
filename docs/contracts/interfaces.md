# Interfaces — alpha-n-naia-144

**상태:** Draft v0.2 (CX+GE 리뷰 반영)
**변경:** Render Backend 분리, font API 정리, scenario API 보완, 에러코드 추가

---

## 모듈 경계

```
┌─────────────────────────────────────────────────┐
│                   Game Logic                     │
│  (월드 상태, 씬 관리, NPC, 입력 매핑)             │
└──────┬──────────────────────┬───────────────────┘
       │                      │
       ▼                      ▼
┌──────────────┐    ┌──────────────────┐
│ Render       │    │ Scenario Module  │
│ Backend      │    │ (텍스트 로딩,     │
│ ┌──────────┐ │    │  압축 해제)       │
│ │ANSI text │ │    └──────────────────┘
│ │backend   │ │
│ ├──────────┤ │    ┌──────────────────┐
│ │Pixel buf │ │    │ Save System      │
│ │backend   │ │    │ (3슬롯, ~1KB)    │
│ └──────────┘ │    └──────────────────┘
└──────────────┘
```

---

## 공통

### 에러 코드

```c
#define OK        0
#define ERR_ARG  -1
#define ERR_IO   -2
#define ERR_MEM  -3
#define ERR_FULL -4
```

---

## 1. Render Backend

```c
typedef enum {
    RENDER_TEXT,
    RENDER_PIXEL
} RenderMode;

typedef struct {
    int width;      // 셀 수 (text) 또는 픽셀 수 (pixel)
    int height;
} ScreenSize;

int render_init(RenderMode mode);
int render_set_mode(RenderMode mode);
int render_flush(void);         // 더블 버퍼 → 실제 출력 (dirty-cell 최적화)
int render_clear(void);
ScreenSize render_size(void);
void render_shutdown(void);
```

### Text Backend

```c
int text_putc(int row, int col, uint32_t ch, uint8_t fg, uint8_t bg);
int text_puts(int row, int col, const char *utf8, uint8_t fg, uint8_t bg);
```

### Pixel Backend

```c
int pixel_set(int x, int y, uint8_t color_idx);
int pixel_fill_rect(int x, int y, int w, int h, uint8_t color_idx);
```

---

## 2. Font / Glyph

```c
typedef struct {
    uint8_t pixels[16];  // 8×16 1bpp
} GlyphBitmap;

const GlyphBitmap *font_get_glyph(uint32_t codepoint);
```

- 입력: Unicode codepoint
- 내부: NFD 분해 후 자모 인덱싱 (한글), 직접 인덱싱 (ASCII/기호)
- 조합형 한글은 초성+중성+종성 → 3개 자모 타일을 인접 셀에 배치

---

## 3. Tileset

```c
typedef struct {
    uint8_t r, g, b;
} Color;

extern const Color PALETTE[16];
extern const uint8_t JAMO_BRIGHTNESS[68];  // 자모 인덱스 → 밝기 0-15 (실측 자동 생성)
```

---

## 4. Game Logic

```c
typedef enum {
    SCENE_TITLE,
    SCENE_CAFE,
    SCENE_ATELIER,
    SCENE_BATTLE,
    SCENE_DIALOG,
    SCENE_ENDING
} SceneType;

int scene_enter(SceneType scene);
int scene_exit(void);
int scene_update(uint32_t delta_ms);
int scene_handle_key(int keycode);
```

---

## 5. Scenario

```c
typedef struct {
    int speaker_id;     // 화자 ID
    int emotion_id;     // 감정 ID (CX DISMISSED portrait → 경량화)
    const char *line;   // 대사 UTF-8
} DialogLine;

int scenario_load(const char *chapter_id);
const DialogLine *scenario_next(void);
int scenario_choose(int option_index);
int scenario_get_choices(const DialogLine **out, int max);
int scenario_reset(void);
int scenario_get_state_id(char *buf, int buf_size);
```

---

## 6. Save System

```c
int save_write(int slot, const char *state_id);
int save_read(int slot, char *state_id, int buf_size);
int save_erase(int slot);
```

---

## 7. Input Keymap

```c
typedef enum {
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_CONFIRM, KEY_CANCEL, KEY_MENU,
    KEY_COUNT
} GameKey;

GameKey input_poll(void);
```

---

## 열린 질문 (잔여)

1. pixel backend에서 타일(8×16) 단위 조작 API가 별도로 필요한가?
2. dirty region 추적을 셀 단위로 할지, 사각형 단위로 할지?
