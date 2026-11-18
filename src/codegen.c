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

/**
 * @brief Prints build-in function subStr
 *
 */
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

	// Local variable definition
	printf("DEFVAR LF@result\n");
	printf("DEFVAR LF@temp1\n");
	printf("DEFVAR LF@temp2\n");
	printf("DEFVAR LF@temp3\n");
	printf("DEFVAR LF@temp4\n");

	// Parameters check
	printf("LT LF@temp1 LF@start int@0\n");
	printf("LT LF@temp2 LF@end int@0\n");
	printf("OR LF@temp3 LF@temp1 LF@temp2\n");

	printf("GT LF@temp1 LF@start LF@end\n");
	printf("OR LF@temp2 LF@temp1 LF@temp3\n");

	printf("STRLEN LF@temp1 LF@value\n");
	printf("GT LF@temp3 LF@start LF@temp1\n");
	printf("OR LF@temp4 LF@temp2 LF@temp3\n");

	printf("EQ LF@temp3 LF@start LF@temp1\n");
	printf("OR LF@temp2 LF@temp4 LF@temp3\n");

	printf("GT LF@temp3 LF@end LF@temp1\n");
	printf("OR LF@temp1 LF@temp2 LF@temp3\n");

	// Wrong parameters (exit value 40)
	printf("JUMPIFNEQ correct$params LF@temp1 bool@true\n");

	printf("POPFRAME\n");
	printf("EXIT int@40\n");

	printf("LABEL correct$params\n");
	printf("MOVE LF@result string@\n");

	// While condition
	printf("LABEL substr$w$0\n");
	printf("LT LF@temp1 LF@start LF@end\n");
	printf("JUMPIFEQ substr$w$0$end LF@temp1 bool@false\n");

	// While code block, going trough the sub string and adding each letter to the previous one
	printf("GETCHAR LF@temp1 LF@value LF@start\n");
	printf("CONCAT LF@result LF@result LF@temp1\n");
	printf("ADD LF@start LF@start int@1\n");
	printf("JUMP substr$w$0\n");

	// Return value
	printf("LABEL substr$w$0$end\n");
	printf("PUSHS LF@result\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

/**
 * @brief Prints build-in function ord
 *
 */
void print_ord()
{
	printf("LABEL ord\n");
	printf("CREATEFRAME\n");
	printf("PUSHFRAME\n");

	// Saving parameters
	printf("DEFVAR LF@value\n");
	printf("DEFVAR LF@index\n");
	printf("POPS LF@index\n");
	printf("POPS LF@value\n");

	// Local variable definition
	printf("DEFVAR LF@result\n");
	printf("DEFVAR LF@temp1\n");
	printf("DEFVAR LF@temp2\n");
	printf("DEFVAR LF@temp3\n");

	// Parameters check
	printf("STRLEN LF@temp1 LF@value\n");
	printf("JUMPIFEQ ord$wrong LF@temp1 int@0\n");

	printf("GT LF@temp2 LF@index LF@temp1\n");
	printf("EQ LF@temp3 LF@index LF@temp1\n");
	printf("OR LF@temp1 LF@temp2 LF@temp3\n");
	printf("JUMPIFEQ ord$wrong LF@temp1 bool@true\n");

	// Save the character
	printf("GETCHAR LF@result LF@value LF@index\n");
	printf("JUMP ord$end\n");

	printf("LABEL ord$wrong\n");
	printf("MOVE LF@result int@-1\n");

	printf("LABEL ord$end\n");
	printf("PUSHS LF@result\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}
/*
	AST_PROGRAM,
	AST_FUNC_DEF,
	AST_PARAM,
	AST_RETURN,
	AST_BLOCK,
	AST_IF,
	AST_WHILE,
	AST_VAR_DEF,

	AST_BIN_OPERATOR,
	AST_UN_OPERATOR,
	AST_ASSIGN,
	AST_CALL,

	AST_INT_TO_DOUBLE,
	AST_EXTENSION_DEF,
	AST_EXTENSION_USE
*/

/**
 * @brief Prints string with chars that need escape sequence
 *
 * @param str String to be printed
 */
void print_string(const char *str)
{
	if (str == NULL)
	{
		return;
	}

	for (int i = 0; str[i] != '\0'; i++)
	{
		unsigned char j = (unsigned char)str[i];
		if (j <= 32 || j == 35 || j == 92 || j >= 127)
		{
			printf("\\03%d", j);
		}
		else
		{
			printf("%c", str[i]);
		}
	}
	printf("\n");
	return;
}

/**
 * @brief Function to generate code for a specific node
 *
 * @param node Node for wich the code should be generated
 */
void generate_node(const ast_node_t *node)
{
	node_type_t type = node->node_type;

	switch (type)
	{
	case AST_LITERAL_INT:
		printf("PUSHS int@%lli", node->ast_node_value.int_literal);
		break;

	case AST_LITERAL_DOUBLE:
		printf("PUSHS float@%a", node->ast_node_value.double_literal);
		break;

	case AST_LITERAL_STRING:
		printf("PUSHS string@");
		print_string(node->ast_node_value.string_literal);
		break;

	case AST_LITERAL_BOOLEAN:
		printf("PUSHS bool@%d", node->ast_node_value.boolean_literal);
		break;

	case AST_LITERAL_UNIT:
		break;

	case AST_IDENTIFIER:
		printf("PUSHS LF@%s", node->ast_node_value.identifier);
		break;

	default:
		break;
	}
}

/**
 * @brief Function for code generation
 *
 * @param root Pointer to node of ast containing the root
 */
void generate_code(const ast_node_t *root)
{
	printf(".IFJcode26\n");

	printf("JUMP main\n");

	print_substr();
	print_ord();

	generate_node(root);
}
