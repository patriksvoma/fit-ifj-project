/**
 * @file codegen.c
 * @brief Source code for implementation of code generation
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-15
 */

#include "codegen.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Prints build-in function subStr
 *
 */
void print_substr()
{
	printf("LABEL subStr\n");
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
	printf("JUMPIFNEQ correct$params$subStr LF@temp1 bool@true\n");

	printf("POPFRAME\n");
	printf("EXIT int@40\n");

	printf("LABEL correct$params$subStr\n");
	printf("MOVE LF@result string@\n");

	// While condition
	printf("LABEL subStr$w$0\n");
	printf("LT LF@temp1 LF@start LF@end\n");
	printf("JUMPIFEQ subStr$w$0$end LF@temp1 bool@false\n");

	// While code block, going trough the sub string and adding each letter to the previous one
	printf("GETCHAR LF@temp1 LF@value LF@start\n");
	printf("CONCAT LF@result LF@result LF@temp1\n");
	printf("ADD LF@start LF@start int@1\n");
	printf("JUMP subStr$w$0\n");

	// Return value
	printf("LABEL subStr$w$0$end\n");
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
	printf("LT LF@temp2 LF@index int@0\n");
	printf("OR LF@temp1 LF@temp1 LF@temp2\n");
	printf("JUMPIFEQ ord$wrong LF@temp1 bool@true\n");

	// Save the character
	printf("STRI2INT LF@result LF@value LF@index\n");
	printf("JUMP ord$end\n");

	printf("LABEL ord$wrong\n");
	printf("MOVE LF@result int@-1\n");

	printf("LABEL ord$end\n");
	printf("PUSHS LF@result\n");
	printf("POPFRAME\n");
	printf("RETURN\n");
}

void print_bool2str()
{
	printf("LABEL bool2str\n");
	printf("PUSHS bool@true\n");
	printf("JUMPIFEQS bool2str$true\n");
	printf("PUSHS string@false\n");
	printf("JUMP bool2str$end\n");
	printf("LABEL bool2str$true\n");
	printf("PUSHS string@true\n");
	printf("LABEL bool2str$end\n");
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
			printf("\\%03d", j);
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
		if (data_type == TYPE_STRING)
		{
			printf("POPS GF@temp2\n");
			printf("POPS GF@temp1\n");
			printf("CONCAT GF@temp1 GF@temp1 GF@temp2\n");
			printf("PUSHS GF@temp1\n");
		}
		else
		{
			printf("ADDS\n");
		}
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

void print_func_def(char *identifier, const char *unique_id, symtable_t *symtables, names_counter *new_func_def)
{
	new_func_def->for_counter = 0;
	new_func_def->if_counter = 0;
	new_func_def->while_counter = 0;

	new_func_def->func_name = identifier;

	// Get data for the function to find parameters
	const symbol_data_t *func_params = symtable_item_find(&symtables[0], identifier);

	if (func_params->data.function.is_main)
	{
		printf("LABEL main\n");
		new_func_def->func_name = "main";
	}
	else
	{
		printf("LABEL %s\n", unique_id);
	}

	printf("CREATEFRAME\n");
	printf("PUSHFRAME\n");

	// Definition and assigment of parameters from the last one, because parameters will be pushed to stack in order
	// when calling the function
	for (int i = func_params->data.function.param_list_count - 1; i >= 0; i--)
	{
		for (int j = func_params->data.function.param_lists[i]->param_count - 1; j >= 0; j--)
		{
			printf("DEFVAR LF@%s$%s\n", unique_id, func_params->data.function.param_lists[i]->params[j]->identifier);
			printf("POPS LF@%s$%s\n", unique_id, func_params->data.function.param_lists[i]->params[j]->identifier);
		}
	}
}

void print_func_call(const char *unique_id, param_node_list_t **param_lists, symtable_t *symtables,
					 names_counter *new_func_def)
{
	const char *id = unique_id;

	if (strcmp(id, "print") == 0)
	{
		if (param_lists[0]->param_count == 0)
		{
			printf("WRITE string@()\n");
		}
		else
		{
			generate_nodes(param_lists[0]->param_node[0], symtables, new_func_def);
			printf("POPS GF@temp1\n");
			printf("WRITE GF@temp1\n");
		}
		printf("PUSHS nil@nil\n");
	}
	// TODO: Nejspis bude potreba predelat
	else if (strcmp(id, "readLine") == 0)
	{
		printf("READ GF@temp1 string\n");
		printf("PUSHS GF@temp1\n");
	}
	else if (strcmp(id, "readInt") == 0)
	{
		printf("READ GF@temp1 int\n");
		printf("PUSHS GF@temp1\n");
	}
	else if (strcmp(id, "readDouble") == 0)
	{
		printf("READ GF@temp1 float\n");
		printf("PUSHS GF@temp1\n");
	}
	else if (strcmp(id, "readBoolean") == 0)
	{
		printf("READ GF@temp1 bool\n");
		printf("PUSHS GF@temp1\n");
	}
	else if (strcmp(id, "toStr") == 0)
	{
		generate_nodes(param_lists[0]->param_node[0], symtables, new_func_def);
		switch (param_lists[0]->param_node[0]->eval_type)
		{
		case TYPE_INT:
			printf("INT2STRS\n");
			break;

		case TYPE_DOUBLE:
			printf("FLOAT2STRS\n");
			break;

		case TYPE_BOOLEAN:
			printf("CALL bool2str\n");
			break;

		case TYPE_UNIT:
			printf("POPS GF@temp1\n");
			printf("PUSHS string@()\n");
			break;

		default:
			break;
		}
	}
}

/*
	AST_FUNC_CALL,
	AST_EXT_DEF,
	AST_EXT_CALL,

	AST_WHILE,
	AST_FOR,
*/

err_t generate_nodes(const ast_node_t *node, symtable_t *symtables, names_counter *new_func_def)
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
		printf("PUSHS nil@nil\n");
		break;

	case AST_VAR_USE:
		printf("PUSHS LF@%s\n", node->variable_use_unique_id);
		break;

	case AST_UN_OPERATOR:
		generate_nodes(node->un_operator.operand, symtables, new_func_def);

		// Multiply by -1
		if (node->un_operator.operator == OP_UMINUS) // TODO: Osetrit double
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
		generate_nodes(node->bin_operator.operand_left, symtables, new_func_def);
		generate_nodes(node->bin_operator.operand_right, symtables, new_func_def);
		print_bin_operation(node->bin_operator.operator, node->eval_type);
		break;

	case AST_VAR_DEF:
		printf("DEFVAR LF@%s\n", node->variable_definition.unique_id);
		generate_nodes(node->variable_definition.assigned_value, symtables, new_func_def);
		printf("POPS LF@%s\n", node->variable_definition.unique_id);
		break;

	case AST_VAR_ASSIGN:
		generate_nodes(node->variable_assignment.assigned_value, symtables, new_func_def);
		printf("POPS LF@%s\n", node->variable_assignment.unique_id);
		break;

	case AST_INT_TO_DOUBLE:
		generate_nodes(node->int2double, symtables, new_func_def);
		printf("INT2FLOATS\n");
		break;

	case AST_BLOCK:
		for (size_t i = 0; i < node->block.statement_count; i++)
		{
			generate_nodes(node->block.statements[i], symtables, new_func_def);
		}
		break;

	case AST_PROGRAM:
		for (size_t i = 0; i < node->program.func_def_count; i++)
		{
			generate_nodes(node->program.func_defs[i], symtables, new_func_def);
		}

		for (size_t i = 0; i < node->program.ext_def_count; i++)
		{
			generate_nodes(node->program.ext_defs[i], symtables, new_func_def);
		}
		break;

	case AST_RETURN_CALL:
		generate_nodes(node->return_call, symtables, new_func_def);
		printf("POPFRAME\n");
		printf("RETURN\n");
		break;

	case AST_FUNC_DEF:
		print_func_def(node->func_def.identificator, node->func_def.unique_id, symtables, new_func_def);
		generate_nodes(node->func_def.block, symtables, new_func_def);

		// If there isnt a return, the frame must be cleaned
		printf("POPFRAME\n");
		printf("RETURN\n");
		break;

	case AST_FUNC_CALL:
		print_func_call(node->func_call.unique_id, node->func_call.param_lists, symtables, new_func_def);
		break;

	case AST_IF:
		unsigned curr = new_func_def->if_counter;
		new_func_def->if_counter++;

		generate_nodes(node->if_statement.condition, symtables, new_func_def);

		// Condition evaluation
		printf("PUSHS bool@true\n");
		printf("JUMPIFNEQS %s$if$%u$else\n", new_func_def->func_name, curr);

		// If block
		generate_nodes(node->if_statement.if_block, symtables, new_func_def);
		printf("JUMP %s$if$%u$end\n", new_func_def->func_name, curr);

		// Else block
		printf("LABEL %s$if$%u$else\n", new_func_def->func_name, curr);
		if (node->if_statement.else_block != NULL)
		{
			generate_nodes(node->if_statement.else_block, symtables, new_func_def);
		}

		printf("LABEL %s$if$%u$end\n", new_func_def->func_name, curr);
		break;

	default:
		break;
	}
	return ERR_OK;
}

err_t generate_code(const ast_node_t *root, const parser_data_t *parser_data)
{
	names_counter *new_func_def = calloc(1, sizeof(names_counter));
	symtable_t *symtables = calloc(2, sizeof(symtable_t));

	if (new_func_def == NULL || symtables == NULL)
	{
		print_err("Failed to allocate memory in generate_nodes()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	symtables[0] = parser_data->function_table;
	symtables[1] = parser_data->extension_table;

	printf(".IFJcode26\n");

	// Variables for coping values on stack
	printf("DEFVAR GF@temp1\n");
	printf("DEFVAR GF@temp2\n");

	printf("CALL main\n");
	printf("EXIT int@0\n");

	print_substr();
	print_ord();
	print_bool2str();

	generate_nodes(root, symtables, new_func_def);

	free(symtables);
	free(new_func_def);
	return ERR_OK;
}
