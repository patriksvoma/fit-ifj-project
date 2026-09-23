# IFJ 2026 project Makefile

.PHONY: build check build-check format clean

CC = gcc-14
CFLAGS = -Wall -Wextra -Werror -Iinclude -MMD -MP

SRC = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)
TARGET = ifj-project

build: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

check:
	clang-format --dry-run --Werror $(SRC) $(HEADERS)

build-check: check build

format:
	clang-format -i $(SRC) $(HEADERS)

clean:
	rm -f $(OBJ) $(DEP) $(TARGET)
