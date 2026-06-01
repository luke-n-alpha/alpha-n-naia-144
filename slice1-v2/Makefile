CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Werror -pedantic -Os -ffunction-sections -fdata-sections
LDFLAGS := -Wl,--gc-sections -s
X11_LIBS := $(shell pkg-config --libs x11)

BUILD_DIR := build
HEADLESS_BIN := $(BUILD_DIR)/naia144_headless
GUI_BIN := $(BUILD_DIR)/naia144_x11
HEADLESS_DUMP := $(BUILD_DIR)/headless_dump.txt

.PHONY: all make_dirs headless test fonttest gametest game-gui audiotest conform clean

all: $(GUI_BIN)

make_dirs:
	mkdir -p $(BUILD_DIR)

$(HEADLESS_BIN): make_dirs main.c core.c core.h platform.h platform_headless.c
	$(CC) $(CFLAGS) -DHEADLESS_BUILD main.c core.c platform_headless.c -o $@ $(LDFLAGS)

$(GUI_BIN): make_dirs main.c core.c core.h platform.h platform_linux_x11.c
	$(CC) $(CFLAGS) main.c core.c platform_linux_x11.c -o $@ $(LDFLAGS) $(X11_LIBS)

headless: $(HEADLESS_BIN)

test: $(HEADLESS_BIN)
	./$(HEADLESS_BIN) > $(HEADLESS_DUMP)
	grep -q '^self_check=PASS$$' $(HEADLESS_DUMP)
	grep -q '^glyph_on_pixels=15$$' $(HEADLESS_DUMP)
	grep -q '^raster_on_pixels=15$$' $(HEADLESS_DUMP)
	grep -q '^brightness=9$$' $(HEADLESS_DUMP)

fonttest: make_dirs font_selftest.c font.c font.h core.c core.h
	$(CC) $(CFLAGS) font_selftest.c font.c core.c -o $(BUILD_DIR)/font_selftest $(LDFLAGS)
	./$(BUILD_DIR)/font_selftest > $(BUILD_DIR)/font_selftest.txt
	grep -q '^font_selftest=PASS$$' $(BUILD_DIR)/font_selftest.txt

# P2 bucket-03 — game logic (FR1 render + FR2 cursor) deterministic acceptance
GAME_BIN := $(BUILD_DIR)/naia144_game_headless
$(GAME_BIN): make_dirs game_headless.c game.c game.h core.c core.h font.c font.h
	$(CC) $(CFLAGS) game_headless.c game.c core.c font.c -o $@ $(LDFLAGS)

gametest: $(GAME_BIN)
	# FR1 (T1): game_init populates 80x25; CAFELUA title => >=7 filled cells
	./$(GAME_BIN) > $(BUILD_DIR)/game_init.txt
	grep -q '^grid_w=80$$' $(BUILD_DIR)/game_init.txt
	grep -q '^grid_h=25$$' $(BUILD_DIR)/game_init.txt
	grep -q '^game_dump=PASS$$' $(BUILD_DIR)/game_init.txt
	grep -q '^cursor_x=40$$' $(BUILD_DIR)/game_init.txt
	grep -q '^cursor_y=12$$' $(BUILD_DIR)/game_init.txt
	test "$$(sed -n 's/^filled_cells=//p' $(BUILD_DIR)/game_init.txt)" -ge 7
	# FR2 (T2): cursor moves. From (40,12): RRRD -> (43,13)
	./$(GAME_BIN) --script "RRRD" > $(BUILD_DIR)/game_move.txt
	grep -q '^cursor_x=43$$' $(BUILD_DIR)/game_move.txt
	grep -q '^cursor_y=13$$' $(BUILD_DIR)/game_move.txt
	# FR2 clamp (mutation-sensitive): 50x LEFT from x=40 clamps to 0, not negative
	./$(GAME_BIN) --script "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL" > $(BUILD_DIR)/game_clamp.txt
	grep -q '^cursor_x=0$$' $(BUILD_DIR)/game_clamp.txt
	grep -q '^cursor_y=12$$' $(BUILD_DIR)/game_clamp.txt
	# FR3 (T3): 5 jamo tiles; selected (under-cursor) tile brightness in 0..15
	grep -q '^tile_count=5$$' $(BUILD_DIR)/game_init.txt
	test "$$(sed -n 's/^sel_brightness=//p' $(BUILD_DIR)/game_init.txt)" -ge 0
	test "$$(sed -n 's/^sel_brightness=//p' $(BUILD_DIR)/game_init.txt)" -le 15
	# FR4 (T4): CONFIRM cycles brightness; 1x changes, 16x wraps back (mod-16, mutation-sensitive)
	./$(GAME_BIN) --seed 7 > $(BUILD_DIR)/g7_init.txt
	./$(GAME_BIN) --seed 7 --script "C" > $(BUILD_DIR)/g7_c1.txt
	./$(GAME_BIN) --seed 7 --script "CCCCCCCCCCCCCCCC" > $(BUILD_DIR)/g7_c16.txt
	test "$$(sed -n 's/^sel_brightness=//p' $(BUILD_DIR)/g7_c1.txt)" != "$$(sed -n 's/^sel_brightness=//p' $(BUILD_DIR)/g7_init.txt)"
	test "$$(sed -n 's/^sel_brightness=//p' $(BUILD_DIR)/g7_c16.txt)" = "$$(sed -n 's/^sel_brightness=//p' $(BUILD_DIR)/g7_init.txt)"
	# FR5 (T5): correct solve for seed 7 -> restore=1 + dialogue; one-short -> restore=0 (mutation-resistant)
	./$(GAME_BIN) --seed 7 --script "LLCCCCCCCRRCCCCCCCCCCRRCCCCCCCCCRRRRCCCC" > $(BUILD_DIR)/g7_solve.txt
	grep -q '^restore_flag=1$$' $(BUILD_DIR)/g7_solve.txt
	grep -q '^dialogue=..*$$' $(BUILD_DIR)/g7_solve.txt
	./$(GAME_BIN) --seed 7 --script "LLCCCCCCCRRCCCCCCCCCCRRCCCCCCCCCRRRRCCC" > $(BUILD_DIR)/g7_short.txt
	grep -q '^restore_flag=0$$' $(BUILD_DIR)/g7_short.txt
	@echo "gametest=PASS"

# P3 — UC1 GUI (game state -> X11 window, arrow/confirm keys). Visual = human gate.
GAME_GUI_BIN := $(BUILD_DIR)/naia144_game_x11
$(GAME_GUI_BIN): make_dirs game_main.c game.c game.h core.c core.h font.c font.h platform.h platform_linux_x11.c
	$(CC) $(CFLAGS) game_main.c game.c core.c font.c platform_linux_x11.c -o $@ $(LDFLAGS) $(X11_LIBS)

game-gui: $(GAME_GUI_BIN)

# NFR6 — procedural audio generation, deterministic self-test (playback = human ear)
audiotest: make_dirs audio.c audio.h audio_selftest.c
	$(CC) $(CFLAGS) audio_selftest.c audio.c -o $(BUILD_DIR)/audio_selftest $(LDFLAGS) -lm
	./$(BUILD_DIR)/audio_selftest > $(BUILD_DIR)/audio_selftest.txt
	grep -q '^audio_selftest=PASS$$' $(BUILD_DIR)/audio_selftest.txt
	@echo "audiotest=PASS"

# Contract<->code conformance gate (deterministic, 0 tokens, no LLM).
# Edit-time enforcement = .agents/hooks/conform-gate.js (PostToolUse). This target
# is the manual/CI sweep. Catches the daf35d0 false-success class (signature-drift
# / contract-only / code-only) that surface gates miss.
conform:
	@python3 conform/check.py --all

clean:
	rm -rf $(BUILD_DIR)
