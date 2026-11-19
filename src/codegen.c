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
		unsigned char j = (unsigned char)str[i]; // Number after \ can be 000 - 255
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
 * @brief Prints code for correct operation
 *
 * @param operator Type of operation
 * @param data_type Data type of operation
 */
void print_bin_operation(operator_type_t operator, data_type_t data_type)
{
	switch (operator)
	{
	case OP_MUL:
		printf("MULS\n");
		break;

	case OP_DIV:
		if (data_type == TYPE_INT)
		{
			printf("IDIVS\n");
		}
		else
		{
			printf("DIVS\n");
		}
		break;

	case OP_PLUS:
		printf("ADDS\n");
		break;

	case OP_MINUS:
		printf("SUBS\n");
		break;

	case OP_LESS:
		printf("LTS\n");
		break;

	case OP_GREATER:
		printf("GTS\n");
		break;

	case OP_EQUAL:
		printf("EQS\n");
		break;

	case OP_NOT_EQUAL:
		printf("EQS\n");
		printf("NOTS\n");
		break;

	case OP_LOGIC_AND:
		printf("ANDS\n");
		break;

	case OP_LOGIC_OR:
		printf("ORS\n");
		break;

	case OP_LESS_EQUAL:
	case OP_GREATER_EQUAL:
		// The values on stack need to be copied to use LTS and EQS
		printf("POPS GF@temp2\n");
		printf("POPS GF@temp1\n");
		printf("PUSHS GF@temp1\n");
		printf("PUSHS GF@temp2\n");

		if (operator == OP_LESS_EQUAL)
		{
			printf("LTS\n");
		}
		else
		{
			printf("GTS\n");
		}

		printf("PUSHS GF@temp1\n");
		printf("PUSHS GF@temp2\n");
		printf("EQS\n");
		printf("ORS\n");
		break;

	default:
		break;
	}
	return;
}

/*
	AST_PROGRAM,
	AST_FUNC_DEF,
	AST_FUNC_CALL,
	AST_EXT_DEF,
	AST_EXT_CALL,
	AST_BLOCK,

	AST_VAR_DEF,
	AST_VAR_ASSIGN,

	AST_RETURN_CALL,
	AST_IF,
	AST_WHILE,
	AST_FOR,
	AST_INT_TO_DOUBLE
*/

/**
 * @brief Function to generate code for a specific node
 *
 * @param node Node for wich the code should be generated
 */
void generate_nodes(const ast_node_t *node)
{
	node_type_t type = node->node_type;

	switch (type)
	{
	case AST_LITERAL_INT:
		printf("PUSHS int@%lli\n", node->literal_int);
		break;

	case AST_LITERAL_DOUBLE:
		printf("PUSHS float@%a\n", node->literal_double);
		break;

	case AST_LITERAL_STRING:
		printf("PUSHS string@");
		print_string(node->literal_string);
		break;

	case AST_LITERAL_BOOLEAN:
		if (node->literal_boolean)
		{
			printf("PUSHS bool@true\n");
		}
		else
		{
			printf("PUSHS bool@false\n");
		}
		break;

	case AST_LITERAL_UNIT:
		break;

	case AST_VAR_USE:
		printf("PUSHS LF@%s\n", node->variable_use);
		break;

	case AST_UN_OPERATOR:
		generate_nodes(node->un_operator.operand);

		// Multiply by -1
		if (node->un_operator.operator == OP_UMINUS)
		{
			printf("PUSHS int@-1\n");
			printf("MULS\n");
		}
		else if (node->un_operator.operator == OP_NEG)
		{
			printf("NOTS\n");
		}
		break; // For unary + value stays the same

	case AST_BIN_OPERATOR:
		generate_nodes(node->bin_operator.operand_left);
		generate_nodes(node->bin_operator.operand_right);
		print_bin_operation(node->bin_operator.operator, node->eval_type);

	default:
		break;
	}
	return;
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

	// Variables for coping values on stack
	printf("DEFVAR GF@temp1\n");
	printf("DEFVAR GF@temp2\n");

	print_substr();
	print_ord();

	generate_nodes(root);
}
