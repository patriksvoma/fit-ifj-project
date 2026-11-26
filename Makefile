# IFJ 2026 project
#
# Authors:
#	xsvomap00 (Patrik Švoma)

.PHONY: build check build-check test e2e-test interpret-test format clean

CC = gcc-14
CFLAGS = -Wall -Wextra -Werror -MMD -MP

SRC = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)
TARGET = ifj-project

TEST_SRC = $(wildcard tests/*.c)
TEST_BIN = $(TEST_SRC:.c=.test)
LIB_SRC = $(filter-out src/main.c,$(SRC))
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
	@status=0; for t in $(TEST_BIN); do ./$$t || status=1; done; exit $$status

tests/%.test: tests/%.c $(LIB_SRC) $(UNITY_DIR)/unity.c
	$(CC) $(CFLAGS) -D UNITY_OUTPUT_COLOR -Isrc -I$(UNITY_DIR) $^ -o $@

# Run compiler integration tests against source fixtures
e2e-test: build
	sh ./tests/e2e.sh

# Run generated IFJcode26 output through an external interpreter
interpret-test: build
	sh ./tests/interpret-out.sh

# Formats all .c and .h files
format:
	clang-format -i $(SRC) $(HEADERS)

# Packs the archive for submission and tests it on merlin
pack: clean
	zip -r xsvomap00.zip src dokumentace.pdf rozdeleni rozsireni Makefile tests
	scp ./xsvomap00.zip merlin:~/xsvomap00.zip
	scp ./is_it_ok.sh merlin:~/is_it_ok.sh
	ssh merlin '\
	mkdir -p tmp_ifj && \
	mv xsvomap00.zip tmp_ifj/xsvomap00.zip && \
	mv is_it_ok.sh tmp_ifj/is_it_ok.sh && \
	cd tmp_ifj && \
	./is_it_ok.sh xsvomap00.zip && \
	exit \
	'

# Compiles the documentation pdf
doc:
	cd docs && latexmk -pdf -synctex=1 -interaction=nonstopmode -file-line-error -outdir=aux -auxdir=aux documentation.tex
	mv docs/aux/documentation.pdf dokumentace.pdf

# Cleans files created during a build
clean:
	rm -rf $(OBJ) $(DEP) $(TARGET) $(TEST_BIN) $(TEST_BIN:.test=.d) $(TEST_BIN:=.test) xsvomap00.zip docs/aux
