#!/bin/sh

# Tento soubor byl napsan pomoci AI
# Model: Copilot Fusion (MAI Code 2 Flash)
# Prompt: Make me a test script to check interpret outputs

set -u

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
COMPILER="$ROOT/ifj-project"
FIXTURES="$ROOT/tests/interpret-out"
TMP_DIR=$(mktemp -d)
# Keep the interpreter outside the repository; IC26INT can select another binary.
INTERPRETER=${IC26INT:-/home/petrm/IFJ/ic26int-linux-x86_64}
trap 'rm -rf "$TMP_DIR"' 0
trap 'exit 1' HUP INT TERM

if [ ! -x "$COMPILER" ]; then
	printf 'Compiler not found or not executable: %s\n' "$COMPILER" >&2
	printf 'Run "make build" first.\n' >&2
	exit 1
fi

if [ -z "$INTERPRETER" ]; then
	printf 'Interpreter is not configured.\n' >&2
	exit 1
fi

set -- $INTERPRETER
INTERPRETER_CMD=$1
shift
INTERPRETER_ARGS=$*
if [ ! -e "$INTERPRETER_CMD" ] && ! command -v "$INTERPRETER_CMD" >/dev/null 2>&1; then
	printf 'Interpreter not found: %s\n' "$INTERPRETER_CMD" >&2
	printf 'Set IC26INT to override the interpreter path.\n' >&2
	exit 1
fi

passed=0
failed=0

if [ -t 1 ] && [ -t 2 ] && [ -z "${NO_COLOR+x}" ] && [ "${TERM:-}" != "dumb" ]; then
	GREEN=$(printf '\033[32m')
	RED=$(printf '\033[31m')
	YELLOW=$(printf '\033[33m')
	BLUE=$(printf '\033[34m')
	RESET=$(printf '\033[0m')
else
	GREEN=''
	RED=''
	YELLOW=''
	BLUE=''
	RESET=''
fi

run_case()
{
	# Arguments: test name, source fixture, stdin, expected stdout, expected exit.
	name=$1
	fixture=$2
	input=$3
	expected_output=$4
	expected_interpreter_exit=${5:-0}

	code_file="$TMP_DIR/$name.ifjcode26"
	stdout_file="$TMP_DIR/$name.stdout"
	stderr_file="$TMP_DIR/$name.stderr"
	expected_file="$TMP_DIR/$name.expected"

	"$COMPILER" < "$FIXTURES/$fixture" > "$code_file" 2> "$stderr_file"
	compiler_exit=$?
	if [ "$compiler_exit" -ne 0 ]; then
		printf '%sFAIL%s: %s (compiler exit %s)\n' "$RED" "$RESET" "$name" "$compiler_exit" >&2
		if [ -s "$stderr_file" ]; then
			cat "$stderr_file" >&2
		fi
		failed=$((failed + 1))
		return
	fi

	# The compiler emits IFJcode26, which is then executed by the interpreter.
	printf '%s' "$input" | "$INTERPRETER_CMD" $INTERPRETER_ARGS "$code_file" > "$stdout_file" 2>> "$stderr_file"
	interpreter_exit=$?

	printf '%s' "$expected_output" > "$expected_file"
	# Compare bytes exactly; print builtins do not add a trailing newline.
	if [ "$interpreter_exit" -eq "$expected_interpreter_exit" ] &&
	   cmp -s "$stdout_file" "$expected_file"; then
		printf '%sPASS%s: %s (output: %s)\n' "$GREEN" "$RESET" "$name" "$expected_output"
		passed=$((passed + 1))
	else
		printf '%sFAIL%s: %s (interpreter exit %s, expected %s)\n' \
			"$RED" "$RESET" "$name" "$interpreter_exit" "$expected_interpreter_exit" >&2
		printf '  expected output: <%s>\n' "$expected_output" >&2
		printf '  actual output:   <' >&2
		cat "$stdout_file" >&2
		printf '>\n' >&2
		if [ -s "$stderr_file" ]; then
			cat "$stderr_file" >&2¨
		fi
		failed=$((failed + 1))
	fi
}

echo "$YELLOW-----Interpret output tests-----$RESET"

run_case "main-def" "main/main.ifj" "" ""

#Build-it print tests
run_case "print-int" "functions/build-in/print-int.ifj" "" "6"
run_case "print-int-var" "functions/build-in/print-int-var.ifj" "" "5"
run_case "print-double" "functions/build-in/print-double.ifj" "" "0x1.a9db22d0e5604p+2"
run_case "print-string" "functions/build-in/print-string.ifj" "" "ahoj svete"
run_case "print-unit" "functions/build-in/print-unit.ifj" "" "()"

#Build-in readLine tests
run_case "readLine" "functions/build-in/readLine.ifj" "ahoj" "ahoj"
run_case "readLine" "functions/build-in/readLine.ifj" "" ""

#Build-in readInt
run_case "readInt" "functions/build-in/readInt.ifj" "10" "10"
run_case "readInt" "functions/build-in/readInt.ifj" "" "0"
run_case "readInt" "functions/build-in/readInt.ifj" "asd10da" "0"
run_case "readInt" "functions/build-in/readInt.ifj" "10__0" "0"

#Build-in readDouble
run_case "readDouble" "functions/build-in/readDouble.ifj" "0x0p+0" "0x0p+0"
run_case "readDouble" "functions/build-in/readDouble.ifj" "10" "0x1.4p+3"
run_case "readDouble" "functions/build-in/readDouble.ifj" "10__" "0x0p+0"
run_case "readDouble" "functions/build-in/readDouble.ifj" "fdh541n" "0x0p+0"
run_case "readDouble" "functions/build-in/readDouble.ifj" "" "0x0p+0"

#Build-in readBoolean
run_case "readBool" "functions/build-in/readBool.ifj" "0" "false"
run_case "readBool" "functions/build-in/readBool.ifj" "1" "false"
run_case "readBool" "functions/build-in/readBool.ifj" "asdtruefs" "false"
run_case "readBool" "functions/build-in/readBool.ifj" "_true" "false"
run_case "readBool" "functions/build-in/readBool.ifj" "true" "true"

printf '\n'
if [ "$failed" -ne 0 ]; then
	printf '%s%d passed, %d failed%s\n' "$RED" "$passed" "$failed" "$RESET" >&2
	exit 1
fi

printf '%s%d passed, %d failed%s\n' "$GREEN" "$passed" "$failed" "$RESET"
