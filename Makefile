# IFJ 2026 project Makefile

.PHONY: build clean

CC = gcc-14
CFLAGS = -Wall -Wextra -Werror

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = ifj-project

build: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
