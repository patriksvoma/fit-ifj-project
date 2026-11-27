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
INTERPRETER=${IC26INT:-"${ROOT}/ic26int-linux-x86_64"}
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
			cat "$stderr_file" >&2
		fi
		failed=$((failed + 1))
	fi
}

echo "$YELLOW-----Interpret output tests-----$RESET"
echo "$BLUE-----main definition-----$RESET"
run_case "main-def" "main/main.ifj" "" ""

echo "$YELLOW-----Built-in functions-----$RESET"
#Build-it print tests
echo "$BLUE-----print-----$RESET"
run_case "print-int" "functions/build-in/print-int.ifj" "" "6"
run_case "print-double" "functions/build-in/print-double.ifj" "" "0x1.a9db22d0e5604p+2"
run_case "print-string" "functions/build-in/print-string.ifj" "" "ahoj svete"
run_case "print-unit" "functions/build-in/print-unit.ifj" "" "()"

#Build-in readLine tests
echo "$BLUE-----readLine-----$RESET"
run_case "readLine" "functions/build-in/readLine.ifj" "ahoj" "ahoj"
run_case "readLine" "functions/build-in/readLine.ifj" "" ""

#Build-in readInt
echo "$BLUE-----readInt-----$RESET"
run_case "readInt" "functions/build-in/readInt.ifj" "10" "10"
run_case "readInt" "functions/build-in/readInt.ifj" "" "0"
run_case "readInt" "functions/build-in/readInt.ifj" "asd10da" "0"
run_case "readInt" "functions/build-in/readInt.ifj" "10__0" "0"

#Build-in readDouble
echo "$BLUE-----readDouble-----$RESET"
run_case "readDouble" "functions/build-in/readDouble.ifj" "0x0p+0" "0x0p+0"
run_case "readDouble" "functions/build-in/readDouble.ifj" "10" "0x1.4p+3"
run_case "readDouble" "functions/build-in/readDouble.ifj" "10__" "0x0p+0"
run_case "readDouble" "functions/build-in/readDouble.ifj" "fdh541n" "0x0p+0"
run_case "readDouble" "functions/build-in/readDouble.ifj" "" "0x0p+0"

#Build-in readBoolean
echo "$BLUE-----readBoolean-----$RESET"
run_case "readBool" "functions/build-in/readBool.ifj" "0" "false"
run_case "readBool" "functions/build-in/readBool.ifj" "1" "false"
run_case "readBool" "functions/build-in/readBool.ifj" "asdtruefs" "false"
run_case "readBool" "functions/build-in/readBool.ifj" "_true" "false"
run_case "readBool" "functions/build-in/readBool.ifj" "true" "true"

#Build-in toStr
echo "$BLUE-----toStr-----$RESET"
run_case "toStr_int" "functions/build-in/toStr-int.ifj" "" "654"
run_case "toStr_double" "functions/build-in/toStr-double.ifj" "" "615.16"
run_case "toStr_string" "functions/build-in/toStr-string.ifj" "" "ahoj"
run_case "toStr_bool" "functions/build-in/toStr-bool.ifj" "" "true"
run_case "toStr_unit" "functions/build-in/toStr-unit.ifj" "" "()"

#Build-in toInt
echo "$BLUE-----toInt-----$RESET"
run_case "toInt" "functions/build-in/toInt.ifj" "" "120"

#Build-in strLength
echo "$BLUE-----strLength-----$RESET"
run_case "strLength" "functions/build-in/strLength.ifj" "ahoj svete" "10"
run_case "strLength" "functions/build-in/strLength.ifj" "" "0"

#Build-in subStr
echo "$BLUE-----subStr-----$RESET"
run_case "subStr" "functions/build-in/subStr.ifj" "ahoj svete" "svete"
run_case "subStr" "functions/build-in/subStr.ifj" "ah" "" "40"
run_case "subStr" "functions/build-in/subStr.ifj" "ahoj svet" "" "40"
run_case "subStr" "functions/build-in/subStr-invalid-start.ifj" "ahoj svete" "" "40"
run_case "subStr" "functions/build-in/subStr-invalid-end.ifj" "ahoj svete" "" "40"
run_case "subStr" "functions/build-in/subStr-invalid-both.ifj" "ahoj svete" "" "40"

#Build-in ord
echo "$BLUE-----ord-----$RESET"
run_case "ord" "functions/build-in/ord.ifj" "ahoj svete" "106"
run_case "ord" "functions/build-in/ord-invalid.ifj" "-1" "-1"
run_case "ord" "functions/build-in/ord-invalid.ifj" "10" "-1"

#Build-in chr
echo "$BLUE-----chr-----$RESET"
run_case "chr" "functions/build-in/chr.ifj" "97" "a"
run_case "chr" "functions/build-in/chr.ifj" "-1" "" "58"
run_case "chr" "functions/build-in/chr.ifj" "200" "" "58"

echo "$YELLOW-----User functions-----$RESET"
#Function definition

#TODO: Pridat vice testu

echo "$BLUE-----function def-----$RESET"
run_case "func_def" "functions/user/func-def.ifj" "" ""
run_case "func_def_more_lists" "functions/user/func-def-lists.ifj" "" ""

#Function calling

#TODO: Pridat vice testu

echo "$BLUE-----function call-----$RESET"
run_case "func_call" "functions/user/func-call.ifj" "" "5"
run_case "func_call" "functions/user/func-call-lists.ifj" "" "-5ahoj"
run_case "func_in_exp" "functions/user/func-in-exp.ifj" "" "25125"

#Function return
echo "$BLUE-----unit function return-----$RESET"
run_case "func_ret" "functions/user/func-ret.ifj" "" "()"
run_case "func_no_ret" "functions/user/func-no-ret.ifj" "" "()"
run_case "func_no_ret_no_exp" "functions/user/func-no-ret-ex.ifj" "" "()"
run_case "func_ret_not_unit" "functions/user/func-not-u.ifj" "" "100"

echo "$YELLOW-----Variables-----$RESET"
run_case "var_def" "vars/var-def.ifj" "" ""
run_case "var_use" "vars/var-use.ifj" "" "ahoj1.657"
run_case "var_op" "vars/var-exp.ifj" "" "-7 8.25 3 4.24 22.78 -3 ahojtrue"
run_case "var_bool_op" "vars/var-bool.ifj" "" "false true false true true true false false true"
#TODO: Testy pro uroven vnoreni

echo "$YELLOW-----If statements-----$RESET"
run_case "if_else" "if/if-else.ifj" "15" "x>10"
run_case "if_else" "if/if-else.ifj" "5" "x<10"
run_case "only_if" "if/only-if.ifj" "15" "x>10 a"
run_case "only_if" "if/only-if.ifj" "5" "a"

echo "$YELLOW-----Extensions-----$RESET"
echo "$BLUE-----valmem-----$RESET"
run_case "valmem" "valmem/valmem.ifj" "" "54554.2454true()ahoj0x1.bp+54"

printf '\n'
if [ "$failed" -ne 0 ]; then
	printf '%s%d passed, %d failed%s\n' "$RED" "$passed" "$failed" "$RESET" >&2
	exit 1
fi

printf '%s%d passed, %d failed%s\n' "$GREEN" "$passed" "$failed" "$RESET"
