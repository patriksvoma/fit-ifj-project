# IFJ 2026 project
#
# Authors:
#	xsvomap00 (Patrik Švoma)

.PHONY: build check build-check format clean

CC = gcc-14
CFLAGS = -Wall -Wextra -Werror -Iinclude -MMD -MP

SRC = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)
TARGET = ifj-project

# Builds the project without any checks
build: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

# Runs a formatting check and static code analysis
check:
	clang-format --dry-run --Werror $(SRC) $(HEADERS)
	cppcheck -I include --enable=style --error-exitcode=1 $(SRC)

# Builds the project, but only if the code passes the check target
build-check: check build

# Formats all .c and .h files
format:
	clang-format -i $(SRC) $(HEADERS)

# Cleans files created during a build
clean:
	rm -f $(OBJ) $(DEP) $(TARGET)
