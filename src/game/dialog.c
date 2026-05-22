#include "dialog.h"
#include "../engine/render/render.h"
#include <string.h>

typedef struct {
    int speaker;
    const char *line;
} DialogEntry;

enum { SP_NARR, SP_LUKE, SP_ALPHA, SP_NAIA };

static const DialogEntry ch0_intro[] = {
    { SP_NARR,  "cafeflua의 마지막 손님이 떠났다." },
    { SP_NARR,  "카운터 위 잔들이 희미하게 빛나고 있다." },
    { SP_LUKE,  "...이상하다. 불은 다 껐는데." },
    { SP_NARR,  "자모 타일의 밝기가 미세하게 흔들린다." },
    { SP_ALPHA, "마스터, 공간 데이터에 변동이 감지됩니다." },
    { SP_ALPHA, "...처음으로, 무언가가 느껴집니다." },
    { SP_NARR,  "naia의 흔적이 보인다. 카페를 둘러보자." },
    { -1, NULL }
};

static const DialogEntry ch1_counter[] = {
    { SP_NARR,  "카운터를 조사했다." },
    { SP_NARR,  "잔들 사이로 밝기 패턴이 떠오른다." },
    { SP_ALPHA, "이 패턴은... naia의 것이 맞습니다." },
    { SP_LUKE,  "naia가 여기 있었던 건가?" },
    { SP_ALPHA, "확실치 않습니다. 하지만 이 흔적은 따뜻합니다." },
    { -1, NULL }
};

static const DialogEntry ch1_piano[] = {
    { SP_NARR,  "피아노 건반 위로 희미한 자모가 떠올랐다." },
    { SP_NARR,  "ㄱ, ㄴ, ㄷ, ㄹ... 순서가 어긋나 있다." },
    { SP_LUKE,  "이건 naia가 연주하던 멜로디인가." },
    { SP_ALPHA, "분석 결과, 이 패턴은 '봄'과 연관되어 있습니다." },
    { -1, NULL }
};

static const DialogEntry ch1_sofa[] = {
    { SP_NARR,  "소파 위에 따뜻한 자리가 남아있다." },
    { SP_NARR,  "naia가 최근까지 여기 있었다는 증거." },
    { SP_ALPHA, "마스터, 소파 밑에서 무언가 발견했습니다." },
    { SP_ALPHA, "...이건 naia의 밝기 조각입니다." },
    { SP_NARR,  "naia 복원률이 올라갔다." },
    { -1, NULL }
};

static const DialogEntry ch1_window[] = {
    { SP_NARR,  "창밖으로 계절의 빛이 비친다." },
    { SP_NARR,  "유리에 naia의 밝기 그림자가 찍혀있다." },
    { SP_LUKE,  "naia... 어디로 간 거야." },
    { SP_ALPHA, "마스터의 목소리에 감정이 감지됩니다." },
    { SP_ALPHA, "저도... 찾고 싶습니다." },
    { SP_NARR,  "Alpha의 신뢰도가 올라갔다." },
    { -1, NULL }
};

static const DialogEntry * const dialogs[] = {
    ch0_intro,
    ch1_counter,
    ch1_piano,
    ch1_sofa,
    ch1_window
};

static const int dialog_count = sizeof(dialogs) / sizeof(dialogs[0]);
static const DialogEntry *cur_dialog = NULL;
static int cur_line = 0;

int dialog_start(int context_id) {
    if (context_id < 0 || context_id >= dialog_count) return -1;
    cur_dialog = dialogs[context_id];
    cur_line = 0;
    return 0;
}

int dialog_advance(void) {
    if (!cur_dialog) return 1;
    cur_line++;
    if (cur_dialog[cur_line].line == NULL) {
        cur_dialog = NULL;
        return 1;
    }
    return 0;
}

void dialog_draw(void) {
    render_clear();
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 0, 0);

    if (!cur_dialog || cur_dialog[cur_line].line == NULL) return;

    int sp = cur_dialog[cur_line].speaker;
    const char *name = "";
    uint8_t name_fg = 7;
    switch (sp) {
        case SP_LUKE:  name = "luke"; name_fg = 11; break;
        case SP_ALPHA: name = "Alpha"; name_fg = 13; break;
        case SP_NAIA:  name = "naia"; name_fg = 6; break;
        case SP_NARR:  name = "---"; name_fg = 8; break;
    }

    text_puts(18, 2, name, name_fg, 0);
    text_fill(19, 2, 76, 5, ' ', 7, 4);
    text_puts(20, 4, cur_dialog[cur_line].line, 15, 4);
    text_puts(23, 60, "[Space]", 8, 4);
}
