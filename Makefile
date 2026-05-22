CC = gcc
CFLAGS = -Os -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=199309L -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections

SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/naia

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/engine/font/font.c \
       $(SRC_DIR)/engine/font/glyph_data.c \
       $(SRC_DIR)/engine/render/render.c \
       $(SRC_DIR)/engine/input/input.c \
       $(SRC_DIR)/game/scene.c \
       $(SRC_DIR)/game/dialog.c \
       $(SRC_DIR)/game/world.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ -lm
	strip $@
	@echo "Size: $$(du -b $@ | cut -f1) bytes"
	@echo "      $$(du -h $@ | cut -f1)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

size: $(TARGET)
	@echo "=== Binary Size ==="
	@du -b $(TARGET) | cut -f1
	@size $(TARGET)

.PHONY: all clean size
