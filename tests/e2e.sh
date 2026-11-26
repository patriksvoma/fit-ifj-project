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
run_case "valid-func-call-before-unit-literal" 0 "functions/valid/valid-func-call-before-unit-literal.ifj"

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

echo "$BLUE-----Parameter redefintions-----$RESET"
run_case "func-param-redefinition-1" 4 "functions/invalid/func-param-redefinition-1.ifj"
run_case "func-param-redefinition-2" 4 "functions/invalid/func-param-redefinition-2.ifj"
run_case "func-param-redefinition-3" 4 "functions/invalid/func-param-redefinition-3.ifj"

echo "$YELLOW-----Built-in functions-----$RESET"
run_case "builtin-print-1" 0 "builtins/valid/builtin-print-1.ifj"
run_case "builtin-print-2" 0 "builtins/valid/builtin-print-2.ifj"
run_case "builtin-print-3" 0 "builtins/valid/builtin-print-3.ifj"
run_case "builtin-print-4" 0 "builtins/valid/builtin-print-4.ifj"
run_case "builtin-print-5" 0 "builtins/valid/builtin-print-5.ifj"

run_case "builtin-readline" 0 "builtins/valid/builtin-readline.ifj"
run_case "builtin-readint" 0 "builtins/valid/builtin-readint.ifj"
run_case "builtin-readdouble" 0 "builtins/valid/builtin-readdouble.ifj"

run_case "builtin-tostr-1" 0 "builtins/valid/builtin-tostr-1.ifj"
run_case "builtin-tostr-2" 0 "builtins/valid/builtin-tostr-2.ifj"
run_case "builtin-tostr-3" 0 "builtins/valid/builtin-tostr-3.ifj"
run_case "builtin-tostr-4" 0 "builtins/valid/builtin-tostr-4.ifj"
run_case "builtin-tostr-5" 0 "builtins/valid/builtin-tostr-5.ifj"

run_case "builtin-toint-valid-1" 0 "builtins/valid/builtin-toint-valid-1.ifj"
run_case "builtin-toint-valid-2" 0 "builtins/valid/builtin-toint-valid-2.ifj"
run_case "builtin-toint-type-1" 5 "builtins/invalid/builtin-toint-type-1.ifj"
run_case "builtin-toint-type-2" 5 "builtins/invalid/builtin-toint-type-2.ifj"
run_case "builtin-toint-type-3" 5 "builtins/invalid/builtin-toint-type-3.ifj"

run_case "builtin-strlength" 0 "builtins/valid/builtin-strlength.ifj"
run_case "builtin-substr" 0 "builtins/valid/builtin-substr.ifj"
run_case "builtin-ord" 0 "builtins/valid/builtin-ord.ifj"
run_case "builtin-chr" 0 "builtins/valid/builtin-chr.ifj"

echo "$YELLOW-----BOOLTHEN-----$RESET"
echo "$BLUE-----If without else-----$RESET"
run_case "boolthen-if-true-no-else" 0 "boolthen/valid/boolthen-if-true-no-else.ifj"
run_case "boolthen-if-false-no-else" 0 "boolthen/valid/boolthen-if-false-no-else.ifj"

echo "$BLUE-----If with else-----$RESET"
run_case "boolthen-if-else.ifj" 0 "boolthen/valid/boolthen-if-else.ifj"
run_case "boolthen-else-if.ifj" 0 "boolthen/valid/boolthen-else-if.ifj"
run_case "boolthen-else-if-else.ifj" 0 "boolthen/valid/boolthen-else-if-else.ifj"
run_case "boolthen-else-if-multiple.ifj" 0 "boolthen/valid/boolthen-else-if-multiple.ifj"

echo "$BLUE-----Invalid conditions-----$RESET"
run_case "boolthen-if-wrong-type-1" 6 "boolthen/invalid/boolthen-if-wrong-type-1.ifj"
run_case "boolthen-if-wrong-type-2" 6 "boolthen/invalid/boolthen-if-wrong-type-2.ifj"
run_case "boolthen-if-wrong-type-3" 6 "boolthen/invalid/boolthen-if-wrong-type-3.ifj"

run_case "boolthen-and-wrong-type-1.ifj" 6 "boolthen/invalid/boolthen-and-wrong-type-1.ifj"
run_case "boolthen-and-wrong-type-2.ifj" 6 "boolthen/invalid/boolthen-and-wrong-type-2.ifj"
run_case "boolthen-and-wrong-type-3.ifj" 6 "boolthen/invalid/boolthen-and-wrong-type-3.ifj"

run_case "boolthen-or-wrong-type-1.ifj" 6 "boolthen/invalid/boolthen-or-wrong-type-1.ifj"
run_case "boolthen-or-wrong-type-2.ifj" 6 "boolthen/invalid/boolthen-or-wrong-type-2.ifj"
run_case "boolthen-or-wrong-type-3.ifj" 6 "boolthen/invalid/boolthen-or-wrong-type-3.ifj"

run_case "boolthen-assignment-wrong-type-1.ifj" 6 "boolthen/invalid/boolthen-assignment-wrong-type-1.ifj"
run_case "boolthen-assignment-wrong-type-2.ifj" 6 "boolthen/invalid/boolthen-assignment-wrong-type-2.ifj"
run_case "boolthen-assignment-wrong-type-3.ifj" 6 "boolthen/invalid/boolthen-assignment-wrong-type-3.ifj"

run_case "boolthen-not-wrong-type-1.ifj" 6 "boolthen/invalid/boolthen-not-wrong-type-1.ifj"
run_case "boolthen-not-wrong-type-2.ifj" 6 "boolthen/invalid/boolthen-not-wrong-type-2.ifj"
run_case "boolthen-not-wrong-type-3.ifj" 6 "boolthen/invalid/boolthen-not-wrong-type-3.ifj"

run_case "boolthen-function-param-wrong-type-1.ifj" 5 "boolthen/invalid/boolthen-function-param-wrong-type-1.ifj"
run_case "boolthen-function-param-wrong-type-2.ifj" 5 "boolthen/invalid/boolthen-function-param-wrong-type-2.ifj"
run_case "boolthen-function-param-wrong-type-3.ifj" 5 "boolthen/invalid/boolthen-function-param-wrong-type-3.ifj"

echo "$BLUE-----Bool values-----$RESET"
run_case "boolthen-bool-variable.ifj" 0 "boolthen/valid/boolthen-bool-variable.ifj"
run_case "boolthen-bool-assignment.ifj" 0 "boolthen/valid/boolthen-bool-assignment.ifj"
run_case "boolthen-bool-comparison.ifj" 0 "boolthen/valid/boolthen-bool-comparison.ifj"

run_case "boolthen-bool-function-param.ifj" 0 "boolthen/valid/boolthen-bool-function-param.ifj"
run_case "boolthen-bool-function-return.ifj" 0 "boolthen/valid/boolthen-bool-function-return.ifj"

echo "$BLUE-----Bool operators-----$RESET"
run_case "boolthen-bool-not-true.ifj" 0 "boolthen/valid/boolthen-bool-not-true.ifj"
run_case "boolthen-bool-not-false.ifj" 0 "boolthen/valid/boolthen-bool-not-false.ifj"
run_case "boolthen-and.ifj" 0 "boolthen/valid/boolthen-and.ifj"
run_case "boolthen-or.ifj" 0 "boolthen/valid/boolthen-or.ifj"
run_case "boolthen-operators.ifj" 0 "boolthen/valid/boolthen-operators.ifj"

echo "$BLUE-----While-----$RESET"
run_case "boolthen-while.ifj" 0 "boolthen/valid/boolthen-while.ifj"

echo "$BLUE-----Built-in functions-----$RESET"
run_case "boolthen-readboolean.ifj" 0 "boolthen/valid/boolthen-readboolean.ifj"
run_case "boolthen-tostr.ifj" 0 "boolthen/valid/boolthen-tostr.ifj"

echo "$YELLOW-----CONTROL-----$RESET"
echo "$BLUE-----If expression-----$RESET"
run_case "control-if-expression-int.ifj" 0 "control/valid/control-if-expression-int.ifj"
run_case "control-if-expression-bool.ifj" 0 "control/valid/control-if-expression-bool.ifj"
run_case "control-if-expression-string.ifj" 0 "control/valid/control-if-expression-string.ifj"
run_case "control-if-expression-multiline.ifj" 0 "control/valid/control-if-expression-multiline.ifj"

echo "$BLUE-----For loops-----$RESET"
run_case "control-for-to.ifj" 0 "control/valid/control-for-to.ifj"
run_case "control-for-by.ifj" 0 "control/valid/control-for-by.ifj"
run_case "control-for-by-negative.ifj" 0 "control/valid/control-for-by-negative.ifj"
run_case "control-for-until.ifj" 0 "control/valid/control-for-until.ifj"

echo "$BLUE-----For guards-----$RESET"
run_case "control-for-guard.ifj" 0 "control/valid/control-for-guard.ifj"
run_case "control-for-guard-expression.ifj" 0 "control/valid/control-for-guard-expression.ifj"
run_case "control-for-multiple-guards.ifj" 0 "control/valid/control-for-multiple-guards.ifj"

echo "$BLUE-----For syntax-----$RESET"
run_case "control-for-single-body.ifj" 0 "control/valid/control-for-single-body.ifj"
run_case "control-for-multiline.ifj" 0 "control/valid/control-for-multiline.ifj"

echo "$YELLOW-----Variables-----$RESET"
echo "$BLUE-----Variable definition-----$RESET"
run_case "var-def-explicit-1" 0 "variables/valid/var-def-explicit-1.ifj"
run_case "var-def-explicit-2" 0 "variables/valid/var-def-explicit-2.ifj"
run_case "var-def-explicit-3" 0 "variables/valid/var-def-explicit-3.ifj"
run_case "var-def-explicit-4" 0 "variables/valid/var-def-explicit-4.ifj"
run_case "var-def-explicit-5" 0 "variables/valid/var-def-explicit-5.ifj"
run_case "var-def-implicit-1" 0 "variables/valid/var-def-implicit-1.ifj"
run_case "var-def-implicit-2" 0 "variables/valid/var-def-implicit-2.ifj"
run_case "var-def-implicit-3" 0 "variables/valid/var-def-implicit-3.ifj"
run_case "var-def-implicit-4" 0 "variables/valid/var-def-implicit-4.ifj"
run_case "var-def-implicit-5" 0 "variables/valid/var-def-implicit-5.ifj"
run_case "var-def-explicit-type-1" 6 "variables/invalid/var-def-explicit-type-1.ifj"
run_case "var-def-explicit-type-2" 6 "variables/invalid/var-def-explicit-type-2.ifj"
run_case "var-def-explicit-type-3" 6 "variables/invalid/var-def-explicit-type-3.ifj"

echo "$BLUE-----Variable assignment-----$RESET"
run_case "val-immutable-assign" 7 "variables/invalid/val-immutable-assign.ifj"
run_case "parameter-immutable-assign" 7 "variables/invalid/parameter-immutable-assign.ifj"
run_case "var-mutable-assign" 0 "variables/valid/var-mutable-assign.ifj"

echo "$BLUE-----Variable usage-----$RESET"


printf '\n'
if [ "$failed" -ne 0 ]; then
printf '%s%d passed, %d failed%s\n' "$RED" "$passed" "$failed" "$RESET" >&2
	exit 1
fi

printf '%s%d passed, %d failed%s\n' "$GREEN" "$passed" "$failed" "$RESET"
