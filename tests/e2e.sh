# Tento soubor byl napsan pomoci AI
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

run_case()
{
	name=$1
	expected=$2
	fixture=$3
	stderr_file="$TMP_DIR/$name.stderr"

	"$COMPILER" < "$FIXTURES/$fixture" > "$TMP_DIR/$name.stdout" 2> "$stderr_file"
	actual=$?

	if [ "$actual" -eq "$expected" ]; then
		printf 'PASS: %s (exit %s)\n' "$name" "$actual"
		passed=$((passed + 1))
	else
		printf 'FAIL: %s (expected exit %s, got %s)\n' "$name" "$expected" "$actual" >&2
		if [ -s "$stderr_file" ]; then
			cat "$stderr_file" >&2
		fi
		failed=$((failed + 1))
	fi
}

# Function calls
run_case "valid-func-call-no-list" 0 "functions/valid/valid-func-call-no-list"
run_case "valid-func-call-one-list-empty" 0 "functions/valid/valid-func-call-one-list-empty"
run_case "valid-func-call-one-list-one-param" 0 "functions/valid/valid-func-call-one-list-one-param"
run_case "valid-func-call-one-list-mul-param" 0 "functions/valid/valid-func-call-one-list-mul-param"
run_case "valid-func-call-mul-list-empty" 0 "functions/valid/valid-func-call-mul-list-empty"
run_case "valid-func-call-mul-list-one-param" 0 "functions/valid/valid-func-call-mul-list-one-param"
run_case "valid-func-call-mul-list-mul-param" 0 "functions/valid/valid-func-call-mul-list-mul-param"
run_case "valid-func-call-mul-list-mix-param" 0 "functions/valid/valid-func-call-mul-list-mix-param"

run_case "func-call-missing-list-1" 2 "functions/invalid/func-call-missing-list-1.ifj"
run_case "func-call-missing-list-2" 2 "functions/invalid/func-call-missing-list-2.ifj"
run_case "func-call-missing-list-3" 2 "functions/invalid/func-call-missing-list-3.ifj"
run_case "func-call-missing-list-4" 2 "functions/invalid/func-call-missing-list-4.ifj"

run_case "func-call-extra-list-1" 2 "functions/invalid/func-call-extra-list-1.ifj"
run_case "func-call-extra-list-2" 2 "functions/invalid/func-call-extra-list-2.ifj"
run_case "func-call-extra-list-3" 2 "functions/invalid/func-call-extra-list-3.ifj"
run_case "func-call-extra-list-4" 2 "functions/invalid/func-call-extra-list-4.ifj"

run_case "func-call-wrong-par-type-1" 2 "functions/invalid/func-call-wrong-par-type-1.ifj"
run_case "func-call-wrong-par-type-2" 2 "functions/invalid/func-call-wrong-par-type-2.ifj"
run_case "func-call-wrong-par-type-3" 2 "functions/invalid/func-call-wrong-par-type-3.ifj"
run_case "func-call-wrong-par-type-4" 2 "functions/invalid/func-call-wrong-par-type-4.ifj"
run_case "func-call-wrong-par-type-5" 2 "functions/invalid/func-call-wrong-par-type-5.ifj"
run_case "func-call-wrong-par-type-6" 2 "functions/invalid/func-call-wrong-par-type-6.ifj"
run_case "func-call-wrong-par-type-7" 2 "functions/invalid/func-call-wrong-par-type-7.ifj"
run_case "func-call-wrong-par-type-8" 2 "functions/invalid/func-call-wrong-par-type-8.ifj"

run_case "func-call-missing-param-1" 2 "functions/invalid/func-call-missing-param-1.ifj"
run_case "func-call-missing-param-2" 2 "functions/invalid/func-call-missing-param-2.ifj"
run_case "func-call-missing-param-3" 2 "functions/invalid/func-call-missing-param-3.ifj"

run_case "func-call-extra-param-1" 2 "functions/invalid/func-call-extra-param-1.ifj"
run_case "func-call-extra-param-2" 2 "functions/invalid/func-call-extra-param-2.ifj"
run_case "func-call-extra-param-3" 2 "functions/invalid/func-call-extra-param-3.ifj"

run_case "func-call-missing-par-type-1" 2 "functions/invalid/func-call-missing-par-type-1.ifj"
run_case "func-call-missing-par-type-2" 2 "functions/invalid/func-call-missing-par-type-2.ifj"
run_case "func-call-missing-par-type-3" 2 "functions/invalid/func-call-missing-par-type-3.ifj"

run_case "func-call-missing-par-name-1" 2 "functions/invalid/func-call-missing-par-name-1.ifj"
run_case "func-call-missing-par-name-2" 2 "functions/invalid/func-call-missing-par-name-2.ifj"
run_case "func-call-missing-par-colon" 2 "functions/invalid/func-call-missing-par-colon.ifj"

run_case "valid-main" 0 "valid-main.ifj"
run_case "invalid-prologue" 2 "invalid-prologue.ifj"
run_case "duplicate-function" 4 "duplicate-function.ifj"
run_case "valid-function-call-and-variable-use" 0 "valid-function-call-and-variable-use.ifj"
run_case "undefined-variable-in-expression" 3 "undefined-variable-in-expression.ifj"
run_case "undefined-function-call" 3 "undefined-function-call.ifj"
run_case "function-call-wrong-arity" 5 "function-call-wrong-arity.ifj"
run_case "function-call-wrong-type" 6 "function-call-wrong-type.ifj"
run_case "function-call-unexpected-empty-list" 5 "function-call-unexpected-empty-list.ifj"
run_case "function-call-omitted-empty-list" 5 "function-call-omitted-empty-list.ifj"

printf '\n%d passed, %d failed\n' "$passed" "$failed"
if [ "$failed" -ne 0 ]; then
	exit 1
fi
