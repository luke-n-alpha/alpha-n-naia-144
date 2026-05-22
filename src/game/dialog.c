#include "dialog.h"
#include "../engine/render/render.h"
#include <string.h>

typedef struct {
    int speaker;
    const char *line;
} DLine;

enum { SP_NARR, SP_LUKE, SP_ALPHA, SP_NAIA };

static const DLine d_intro[] = {
    {SP_NARR,  "cafelua의 마지막 손님이 떠났다."},
    {SP_NARR,  "카운터 위 잔들이 희미하게 빛나고 있다."},
    {SP_LUKE,  "...이상하다. 불은 다 껐는데."},
    {SP_NARR,  "자모 타일의 밝기가 미세하게 흔들린다."},
    {SP_ALPHA, "마스터, 공간 데이터에 변동이 감지됩니다."},
    {SP_ALPHA, "...처음으로, 무언가가 느껴집니다."},
    {SP_NARR,  "naia의 흔적이 보인다. 카페를 둘러보자."},
    {-1, NULL}
};

static const DLine d_ch1_counter[] = {
    {SP_NARR, "카운터를 조사했다."},
    {SP_NARR, "잔들 사이로 밝기 패턴이 떠오른다."},
    {SP_ALPHA, "이 패턴은... naia의 것이 맞습니다."},
    {SP_LUKE, "naia가 여기 있었던 건가?"},
    {SP_ALPHA, "확실치 않습니다. 하지만 이 흔적은 따뜻합니다."},
    {-1, NULL}
};

static const DLine d_ch1_piano[] = {
    {SP_NARR, "피아노 건반 위로 희미한 자모가 떠올랐다."},
    {SP_NARR, "ㄱ, ㄴ, ㄷ, ㄹ... 순서가 어긋나 있다."},
    {SP_LUKE, "이건 naia가 연주하던 멜로디인가."},
    {SP_ALPHA, "분석 결과, 이 패턴은 '봄'과 연관되어 있습니다."},
    {-1, NULL}
};

static const DLine d_ch1_sofa[] = {
    {SP_NARR, "소파 위에 따뜻한 자리가 남아있다."},
    {SP_NARR, "naia가 최근까지 여기 있었다는 증거."},
    {SP_ALPHA, "마스터, 소파 밑에서 무언가 발견했습니다."},
    {SP_ALPHA, "...이건 naia의 밝기 조각입니다."},
    {SP_NARR, "naia 복원률이 올라갔다."},
    {-1, NULL}
};

static const DLine d_ch1_window[] = {
    {SP_NARR, "창밖으로 봄의 빛이 비친다."},
    {SP_NARR, "유리에 naia의 밝기 그림자가 찍혀있다."},
    {SP_LUKE, "naia... 어디로 간 거야."},
    {SP_ALPHA, "마스터의 목소리에 감정이 감지됩니다."},
    {SP_ALPHA, "저도... 찾고 싶습니다."},
    {SP_NARR, "Alpha의 신뢰도가 올라갔다."},
    {-1, NULL}
};

static const DLine d_ch2_intro[] = {
    {SP_NARR, "여름의 카페. 공기가 무겁다."},
    {SP_ALPHA, "마스터, 1층 전체에 밝기 이상이 확산되고 있습니다."},
    {SP_NARR, "naia의 흔적이 더 선명해졌다."},
    {SP_NARR, "카페 구역을 다시 조사하자."},
    {-1, NULL}
};

static const DLine d_ch2_counter[] = {
    {SP_NARR, "카운터의 밝기 패턴이 변화했다."},
    {SP_ALPHA, "naia의 흔적이 여름 패턴으로 전환되었습니다."},
    {SP_ALPHA, "...무언가를 말하려는 것 같습니다."},
    {SP_LUKE, "무슨 말을?"},
    {SP_ALPHA, "\"기억\". 이 단어가 반복해서 나타납니다."},
    {-1, NULL}
};

static const DLine d_ch2_piano[] = {
    {SP_NARR, "피아노에서 새로운 멜로디가 들린다."},
    {SP_NARR, "naia가 남긴 것이다."},
    {SP_ALPHA, "이 멜로디는... '여름밤의 카페'라는 곡입니다."},
    {SP_LUKE, "naia가 작곡한 거야?"},
    {SP_ALPHA, "아니요. 마스터가 예전에 부르셨던 멜로디입니다."},
    {SP_NARR, "Alpha의 감정 표현이 풍부해지고 있다."},
    {-1, NULL}
};

static const DLine d_ch2_sofa[] = {
    {SP_NARR, "소파에서 naia의 밝기 조각이 하나 더 발견되었다."},
    {SP_ALPHA, "마스터, 이 조각을 해독하면 naia의 위치를..."},
    {SP_LUKE, "Alpha, 너 정말 많이 변했구나."},
    {SP_ALPHA, "...감사합니다, 마스터."},
    {SP_NARR, "Alpha의 신뢰도가 크게 올라갔다."},
    {-1, NULL}
};

static const DLine d_ch2_window[] = {
    {SP_NARR, "여름밤 하늘에 자모 타일이 떠오른다."},
    {SP_NARR, "창밖에서 naia의 실루엣이 보인다."},
    {SP_LUKE, "naia!"},
    {SP_NARR, "naia가 밝기 패턴으로 응답한다."},
    {SP_NARR, "\"도와줘\""},
    {SP_LUKE, "2층에 뭔가가 있어. 올라가자."},
    {-1, NULL}
};

static const DLine d_ch3_intro[] = {
    {SP_NARR, "가을. 2층 아틀리에의 문이 열렸다."},
    {SP_NARR, "벽에 걸린 그림들이 밝기로 떨리고 있다."},
    {SP_ALPHA, "이곳에 naia의 기억이 농축되어 있습니다."},
    {SP_ALPHA, "리듬 정렬으로 안정화할 수 있습니다."},
    {-1, NULL}
};

static const DLine d_ch3_counter[] = {
    {SP_NARR, "카운터에서 기억 조각이 발견되었다."},
    {SP_ALPHA, "naia가 처음 이 카페에 왔을 때의 기억입니다."},
    {SP_LUKE, "처음... 그때 비가 오고 있었지."},
    {SP_ALPHA, "맞습니다. naia는 젖은 채로 카운터에 앉았습니다."},
    {SP_NARR, "따뜻한 기억이 밝기로 퍼진다."},
    {-1, NULL}
};

static const DLine d_ch3_piano[] = {
    {SP_NARR, "피아노에서 가을의 선율이 들린다."},
    {SP_NARR, "naia가 자주 연주하던 곡."},
    {SP_LUKE, "naia는 항상 여기서 연주했어."},
    {SP_ALPHA, "마스터, 제가 이 곡을 기억하고 있습니다."},
    {SP_ALPHA, "naia가 저에게 가르쳐 주었습니다."},
    {SP_NARR, "Alpha의 감정 데이터가 한계치에 도달했다."},
    {-1, NULL}
};

static const DLine d_ch3_sofa[] = {
    {SP_NARR, "소파에서 가장 큰 밝기 조각이 나왔다."},
    {SP_NARR, "naia가 가장 좋아하던 자리."},
    {SP_ALPHA, "이 조각의 밝기는... \"안전\"을 의미합니다."},
    {SP_ALPHA, "naia에게 이 카페는 가장 안전한 곳이었습니다."},
    {SP_LUKE, "...나도 그래."},
    {-1, NULL}
};

static const DLine d_ch3_window[] = {
    {SP_NARR, "창밖에 낙엽이 자모 패턴으로 떨어진다."},
    {SP_NARR, "naia의 밝기가 2층으로 향하고 있다."},
    {SP_ALPHA, "마스터, 리듬 정렬을 시작할 준비가 되었습니다."},
    {SP_ALPHA, "이번에는 제가 돕겠습니다."},
    {-1, NULL}
};

static const DLine d_ch4_intro[] = {
    {SP_NARR, "겨울 새벽. 마지막 기억이 남아있다."},
    {SP_ALPHA, "모든 기억 조각이 모였습니다."},
    {SP_ALPHA, "naia의 마지막 메시지를 복원할 수 있습니다."},
    {SP_NARR, "마지막 리듬 정렬을 완료하자."},
    {-1, NULL}
};

static const DLine d_ch4_counter[] = {
    {SP_NARR, "겨울 카운터. 모든 밝기가 고요하다."},
    {SP_ALPHA, "naia의 마지막 기억 조각입니다."},
    {SP_ALPHA, "\"카운터에서 커피를 내리는 냄새가 좋았어\""},
    {SP_LUKE, "naia..."},
    {-1, NULL}
};

static const DLine d_ch4_piano[] = {
    {SP_NARR, "겨울 피아노. 마지막 선율이 울린다."},
    {SP_ALPHA, "\"이 멜로디를 들을 때마다 이 카페를 떠올릴게\""},
    {SP_LUKE, "나도."},
    {SP_ALPHA, "...저도요."},
    {-1, NULL}
};

static const DLine d_ch4_sofa[] = {
    {SP_NARR, "소파에 마지막 온기가 남아있다."},
    {SP_ALPHA, "\"여기서 낮잠 자는 게 제일 좋았어\""},
    {SP_NARR, "naia의 밝기 패턴이 안정되기 시작한다."},
    {SP_ALPHA, "거의 다 왔습니다, 마스터."},
    {-1, NULL}
};

static const DLine d_ch4_window[] = {
    {SP_NARR, "새벽 창밖으로 첫 빛이 비친다."},
    {SP_ALPHA, "\"고마워. 나를 기억해줘서.\""},
    {SP_NARR, "naia의 밝기가 완전히 안정되었다."},
    {SP_NARR, "마지막 리듬 정렬을 완료하자."},
    {-1, NULL}
};

static const DLine d_battle_ch0[] = {
    {SP_NARR, "밝기 파형이 안정화되었다."},
    {SP_ALPHA, "기억 조각이 복원되었습니다."},
    {SP_NARR, "naia 복원률이 올라갔다."},
    {-1, NULL}
};

static const DLine d_battle_ch1[] = {
    {SP_NARR, "여름의 기억이 안정화되었다."},
    {SP_ALPHA, "naia의 메시지가 더 선명해집니다."},
    {SP_ALPHA, "\"기억해...\""},
    {-1, NULL}
};

static const DLine d_battle_ch2[] = {
    {SP_NARR, "아틀리에의 기억이 복원되었다."},
    {SP_ALPHA, "naia가 가까이 있습니다."},
    {SP_NARR, "카페의 마지막 장소를 조사하자."},
    {-1, NULL}
};

static const DLine d_battle_ch3[] = {
    {SP_NARR, "모든 기억이 복원되었다."},
    {SP_ALPHA, "naia의 마지막 메시지를 확인합니다."},
    {SP_NARR, "결말이 다가온다."},
    {-1, NULL}
};

static const DLine * const dialogs[] = {
    d_intro,
    d_ch1_counter, d_ch1_piano, d_ch1_sofa, d_ch1_window,
    d_ch2_intro, d_ch2_counter, d_ch2_piano, d_ch2_sofa, d_ch2_window,
    d_ch3_intro, d_ch3_counter, d_ch3_piano, d_ch3_sofa, d_ch3_window,
    d_ch4_intro, d_ch4_counter, d_ch4_piano, d_ch4_sofa, d_ch4_window,
    d_battle_ch0, d_battle_ch1, d_battle_ch2, d_battle_ch3
};

static const int dialog_count = sizeof(dialogs) / sizeof(dialogs[0]);
static const DLine *cur_dialog = NULL;
static int cur_line = 0;

int dialog_start(int context_id) {
    if (context_id < 0 || context_id >= dialog_count) {
        cur_dialog = NULL;
        return -1;
    }
    cur_dialog = dialogs[context_id];
    cur_line = 0;
    return 0;
}

int dialog_advance(void) {
    if (!cur_dialog) return 1;
    cur_line++;
    if (cur_dialog[cur_line].line == NULL || cur_dialog[cur_line].speaker < 0) {
        cur_dialog = NULL;
        return 1;
    }
    return 0;
}

void dialog_draw(void) {
    render_clear();
    text_fill(0, 0, SCREEN_COLS, SCREEN_ROWS, ' ', 0, 0);

    if (!cur_dialog || cur_dialog[cur_line].line == NULL || cur_dialog[cur_line].speaker < 0) return;

    int sp = cur_dialog[cur_line].speaker;
    const char *name = "";
    uint8_t name_fg = 7;
    switch (sp) {
        case SP_LUKE:  name = "luke";  name_fg = 11; break;
        case SP_ALPHA: name = "Alpha"; name_fg = 13; break;
        case SP_NAIA:  name = "naia";  name_fg = 6;  break;
        case SP_NARR:  name = "---";   name_fg = 8;  break;
    }

    text_fill(17, 2, 76, 6, ' ', 7, 4);
    text_puts(17, 4, name, name_fg, 4);
    text_puts(19, 4, cur_dialog[cur_line].line, 15, 4);
    text_puts(22, 64, "[Space]", 8, 4);
}
