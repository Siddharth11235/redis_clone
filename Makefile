CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

SRCS = $(wildcard $(SRC_DIR)/**/*.c $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = redis_clone

all: $(TARGET)

$(TARGET): $(OBJS)
    $(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
    mkdir -p $(dir $@)
    $(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
    rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean