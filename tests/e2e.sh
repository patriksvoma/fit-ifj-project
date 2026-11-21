# Tento soubor byl puvodne napsan pomoci AI (nasledne upraven)
# Model: Copilot Fusion (gp6-luna)
# Prompt: can you write me a simple extendable script with a few test cases

#!/bin/sh

set -u

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
COMPILER="$ROOT/ifj-project"
FIXTURES="$ROOT/tests/e2e"
TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' 0
trap 'exit 1' HUP INT TERM

if [ ! -x "$COMPILER" ]; then
	printf 'Compiler not found or not executable: %s\n' "$COMPILER" >&2
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
	name=$1
	expected=$2
	fixture=$3
	stderr_file="$TMP_DIR/$name.stderr"

	"$COMPILER" < "$FIXTURES/$fixture" > "$TMP_DIR/$name.stdout" 2> "$stderr_file"
	actual=$?

	if [ "$actual" -eq "$expected" ]; then
		printf '%sPASS%s: %s (exit %s)\n' "$GREEN" "$RESET" "$name" "$actual"
		passed=$((passed + 1))
	else
		printf '%sFAIL%s: %s (expected exit %s, got %s)\n' "$RED" "$RESET" "$name" "$expected" "$actual" >&2
		if [ -s "$stderr_file" ]; then
			cat "$stderr_file" >&2
		fi
		failed=$((failed + 1))
	fi
}

# Main
echo "$YELLOW-----Generic tests-----$RESET"
run_case "valid-main" 0 "main/valid/valid-main.ifj"
run_case "invalid-prologue" 2 "main/invalid/invalid-prologue.ifj"

# Function calls
echo "$YELLOW-----Function definitions and calls-----$RESET"
echo "$BLUE-----Parameter and parameter lists-----$RESET"
run_case "valid-func-call-no-list" 0 "functions/valid/valid-func-call-no-list.ifj"
run_case "valid-func-call-one-list-empty" 0 "functions/valid/valid-func-call-one-list-empty.ifj"
run_case "valid-func-call-one-list-one-param" 0 "functions/valid/valid-func-call-one-list-one-param.ifj"
run_case "valid-func-call-one-list-mul-param" 0 "functions/valid/valid-func-call-one-list-mul-param.ifj"
run_case "valid-func-call-mul-list-empty" 0 "functions/valid/valid-func-call-mul-list-empty.ifj"
run_case "valid-func-call-mul-list-one-param" 0 "functions/valid/valid-func-call-mul-list-one-param.ifj"
run_case "valid-func-call-mul-list-mul-param" 0 "functions/valid/valid-func-call-mul-list-mul-param.ifj"
run_case "valid-func-call-mul-list-mix-param" 0 "functions/valid/valid-func-call-mul-list-mix-param.ifj"

run_case "func-call-missing-list-1" 5 "functions/invalid/func-call-missing-list-1.ifj"
run_case "func-call-missing-list-2" 5 "functions/invalid/func-call-missing-list-2.ifj"
run_case "func-call-missing-list-3" 5 "functions/invalid/func-call-missing-list-3.ifj"
run_case "func-call-missing-list-4" 5 "functions/invalid/func-call-missing-list-4.ifj"

run_case "func-call-extra-list-1" 5 "functions/invalid/func-call-extra-list-1.ifj"
run_case "func-call-extra-list-2" 5 "functions/invalid/func-call-extra-list-2.ifj"
run_case "func-call-extra-list-3" 5 "functions/invalid/func-call-extra-list-3.ifj"
run_case "func-call-extra-list-4" 5 "functions/invalid/func-call-extra-list-4.ifj"

echo "$BLUE-----Parameter types-----$RESET"
run_case "func-call-wrong-par-type-1" 5 "functions/invalid/func-call-wrong-par-type-1.ifj"
run_case "func-call-wrong-par-type-2" 5 "functions/invalid/func-call-wrong-par-type-2.ifj"
run_case "func-call-wrong-par-type-3" 5 "functions/invalid/func-call-wrong-par-type-3.ifj"
run_case "func-call-wrong-par-type-4" 5 "functions/invalid/func-call-wrong-par-type-4.ifj"
run_case "func-call-wrong-par-type-5" 5 "functions/invalid/func-call-wrong-par-type-5.ifj"
run_case "func-call-wrong-par-type-6" 5 "functions/invalid/func-call-wrong-par-type-6.ifj"
run_case "func-call-wrong-par-type-7" 5 "functions/invalid/func-call-wrong-par-type-7.ifj"
run_case "func-call-wrong-par-type-8" 5 "functions/invalid/func-call-wrong-par-type-8.ifj"

echo "$BLUE-----Parameter counts-----$RESET"
run_case "func-call-missing-param-1" 5 "functions/invalid/func-call-missing-param-1.ifj"
run_case "func-call-missing-param-2" 5 "functions/invalid/func-call-missing-param-2.ifj"
run_case "func-call-missing-param-3" 5 "functions/invalid/func-call-missing-param-3.ifj"

run_case "func-call-extra-param-1" 5 "functions/invalid/func-call-extra-param-1.ifj"
run_case "func-call-extra-param-2" 5 "functions/invalid/func-call-extra-param-2.ifj"
run_case "func-call-extra-param-3" 5 "functions/invalid/func-call-extra-param-3.ifj"

run_case "func-call-missing-par-type-1" 2 "functions/invalid/func-call-missing-par-type-1.ifj"
run_case "func-call-missing-par-type-2" 2 "functions/invalid/func-call-missing-par-type-2.ifj"
run_case "func-call-missing-par-type-3" 2 "functions/invalid/func-call-missing-par-type-3.ifj"

run_case "func-call-missing-par-name-1" 2 "functions/invalid/func-call-missing-par-name-1.ifj"
run_case "func-call-missing-par-name-2" 2 "functions/invalid/func-call-missing-par-name-2.ifj"
run_case "func-call-missing-par-colon" 2 "functions/invalid/func-call-missing-par-colon.ifj"

echo "$BLUE-----Function definitions-----$RESET"
run_case "func-call-def-later-1" 0 "functions/valid/func-call-def-later-1.ifj"
run_case "func-call-def-later-2" 0 "functions/valid/func-call-def-later-2.ifj"

run_case "func-redefinition-1" 4 "functions/invalid/func-redefinition-1.ifj"
run_case "func-redefinition-2" 4 "functions/invalid/func-redefinition-2.ifj"
run_case "func-redefinition-3" 4 "functions/invalid/func-redefinition-3.ifj"

run_case "func-call-undefined-1" 3 "functions/invalid/func-call-undefined-1.ifj"
run_case "func-call-undefined-2" 3 "functions/invalid/func-call-undefined-2.ifj"

echo "$BLUE-----Main function and annotiation-----$RESET"
run_case "func-missing-main-1" 3 "functions/invalid/func-missing-main-1.ifj"
run_case "func-missing-main-2" 3 "functions/invalid/func-missing-main-2.ifj"

run_case "func-multiple-main-1" 4 "functions/invalid/func-multiple-main-1.ifj"
run_case "func-multiple-main-2" 4 "functions/invalid/func-multiple-main-2.ifj"

run_case "func-main-space" 1 "functions/invalid/func-main-space.ifj"

run_case "func-main-same-line" 0 "functions/valid/func-main-same-line.ifj"
run_case "func-main-diff-line" 0 "functions/valid/func-main-diff-line.ifj"

run_case "func-main-wrong-annotation-1" 1 "functions/invalid/func-main-wrong-annotation-1.ifj"
run_case "func-main-wrong-annotation-2" 1 "functions/invalid/func-main-wrong-annotation-2.ifj"

run_case "func-main-wrong-return-type-1" 6 "functions/invalid/func-main-wrong-return-type-1.ifj"
run_case "func-main-wrong-return-type-2" 6 "functions/invalid/func-main-wrong-return-type-2.ifj"
run_case "func-main-wrong-return-type-3" 6 "functions/invalid/func-main-wrong-return-type-3.ifj"
run_case "func-main-wrong-return-type-4" 6 "functions/invalid/func-main-wrong-return-type-4.ifj"

run_case "func-main-return-type-1" 0 "functions/valid/func-main-return-type-1.ifj"
run_case "func-main-return-type-2" 0 "functions/valid/func-main-return-type-2.ifj"

run_case "func-main-parameter-1" 5 "functions/invalid/func-main-parameter-1.ifj"
run_case "func-main-parameter-2" 5 "functions/invalid/func-main-parameter-2.ifj"

echo "$BLUE-----Unit type return-----$RESET"
run_case "func-unit-empty-return" 6 "functions/invalid/func-unit-empty-return.ifj"
run_case "func-unit-explicit-return" 0 "functions/valid/func-unit-explicit-return.ifj"

printf '\n'
if [ "$failed" -ne 0 ]; then
printf '%s%d passed, %d failed%s\n' "$RED" "$passed" "$failed" "$RESET" >&2
	exit 1
fi

printf '%s%d passed, %d failed%s\n' "$GREEN" "$passed" "$failed" "$RESET"
