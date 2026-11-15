/**
 * @file codegen.c
 * @brief Source code for implementation of code generation
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-15
 */

#include "ast.h"
#include <stdio.h>

// Substr fuction definition
void print_substr()
{
	printf("LABEL substr\n");
	printf("CREATEFRAME\n");
	printf("PUSHFRAME\n");

	// Saving parameters
	printf("DEFVAR LF@value\n");
	printf("DEFVAR LF@start\n");
	printf("DEFVAR LF@end\n");
	printf("POPS LF@end\n");
	printf("POPS LF@start\n");
	printf("POPS LF@value\n");

	// Local variables
	printf("DEFVAR LF@result\n");
	printf("DEFVAR LF@temp\n");
	printf("MOVE LF@result string@\n");

	// While condition
	printf("LABEL substr$w$0\n");
	printf("LT LF@temp LF@start LF@end\n");
	printf("JUMPIFEQ substr$w$0$end LF@temp bool@false\n");

	// While code block
	printf("GETCHAR LF@temp LF@value LF@start\n");
	printf("CONCAT LF@result LF@result LF@temp\n");
	printf("ADD LF@start LF@start int@1\n");
	printf("JUMP substr$w$0\n");

	printf("LABEL substr$w$0$end\n");
	printf("PUSHS LF@result\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

/**
 * @brief Function for code generation
 *
 * @param root Pointer to node of ast containing the root
 */
void generate_code(ast_node_t *root)
{
	// TODO: remove
	(void)root;

	printf(".IFJcode26\n");

	printf("JUMP main\n");

	print_substr();
}
