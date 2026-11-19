# IFJ 2026 project
#
# Authors:
#	xsvomap00 (Patrik Švoma)

.PHONY: build check build-check test test-e2e format clean

CC = gcc-14
CFLAGS = -Wall -Wextra -Werror -MMD -MP

SRC = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)
TARGET = ifj-project

TEST_SRC = $(wildcard tests/*.c)
TEST_BIN = tests/test_runner
UNITY_DIR = third-party/Unity/src

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
	cppcheck --enable=style --inline-suppr --error-exitcode=1 $(SRC)

# Builds the project, but only if the code passes the check target
build-check: check build

# Run tests
test: $(TEST_BIN)
	./$(TEST_BIN)

# Run compiler integration tests against source fixtures
test-e2e: build
	sh ./tests/e2e.sh

$(TEST_BIN): $(TEST_SRC) $(filter-out src/main.c,$(SRC)) $(UNITY_DIR)/unity.c
	$(CC) $(CFLAGS) -D UNITY_OUTPUT_COLOR -Isrc -I$(UNITY_DIR) $^ -o $@

# Formats all .c and .h files
format:
	clang-format -i $(SRC) $(HEADERS)

# Cleans files created during a build
clean:
	rm -f $(OBJ) $(DEP) $(TARGET) $(TEST_BIN) "tests/test_runner.d"
