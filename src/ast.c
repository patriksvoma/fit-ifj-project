/**
 * @file ast.c
 * @brief Ast functions definition
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-12
 */

#include "ast.h"

/*ast_node_t *ast_node_init(ast_node_t *node) {
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}
	else
	{
		return node;
	}
}*/

/*
	AST_PROGRAM,
	AST_FUNC_DEF,
	AST_BLOCK,
	AST_IF,
	AST_WHILE,
	AST_CALL,
*/

ast_node_t *ast_add_literal(literal_t *literal)
{
	ast_node_t *leaf;
	leaf = malloc(sizeof(ast_node_t));

	if (leaf == NULL)
	{
		return NULL;
	}

	// Switch statement to add correct type and value to leaf
	switch (literal->literal_type)
	{
	case TYPE_INT:
		leaf->node_type = AST_LITERAL_INT;
		leaf->eval_type = literal->literal_type;
		leaf->ast_node_value.int_literal = literal->value.integer;
		break;

	case TYPE_DOUBLE:
		leaf->node_type = AST_LITERAL_DOUBLE;
		leaf->eval_type = literal->literal_type;
		leaf->ast_node_value.double_literal = literal->value.decimal;
		break;

	case TYPE_STRING:
		leaf->node_type = AST_LITERAL_STRING;
		leaf->eval_type = literal->literal_type;
		leaf->ast_node_value.string_literal = literal->value.string;
		break;

	case TYPE_BOOLEAN:
		leaf->node_type = AST_LITERAL_BOOLEAN;
		leaf->eval_type = literal->literal_type;
		leaf->ast_node_value.boolean_literal = literal->value.boolean;
		break;

	default:
		leaf->node_type = AST_LITERAL_UNIT;
		leaf->eval_type = literal->literal_type;
		break;
	}

	return leaf;
}

ast_node_t *ast_add_identifier(char *i, data_type_t i_type)
{
	ast_node_t *leaf;
	leaf = malloc(sizeof(ast_node_t));

	if (leaf == NULL)
	{
		return NULL;
	}

	// Set values
	leaf->eval_type = i_type;
	leaf->node_type = AST_IDENTIFIER;
	leaf->ast_node_value.identifier = i;

	return leaf;
}

ast_node_t *ast_add_unary_op(ast_node_t *operand, operator_type_t op)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	if (op == OP_NEG || op == OP_UMINUS || op == OP_UPLUS)
	{
		// Setting values
		node->eval_type = operand->eval_type;
		node->node_type = AST_UN_OPERATOR;
		node->ast_node_value.un_operator.operand = operand;
		node->ast_node_value.un_operator.operator_type = op;
		return node;
	}
	else // Binary operations
	{
		free(node);
		return NULL;
	}
}

ast_node_t *ast_add_binary_op(ast_node_t *operand_left, ast_node_t *operand_right, operator_type_t op)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	if (op != OP_NEG && op != OP_UMINUS && op != OP_UPLUS)
	{
		// Setting values
		node->eval_type = operand_left->eval_type; // TODO: Vyresit zmeny typu po vyhodnocovani
		node->node_type = AST_BIN_OPERATOR;
		node->ast_node_value.bin_operator.operand_left = operand_left;
		node->ast_node_value.bin_operator.operand_right = operand_right;
		node->ast_node_value.bin_operator.operator_type = op;
		return node;
	}
	else // Unary operations
	{
		free(node);
		return NULL;
	}
}

ast_node_t *ast_add_assign(char *i, data_type_t i_type, ast_node_t *expression)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = i_type;
	node->node_type = AST_ASSIGN;
	node->ast_node_value.assign.expression = expression;
	node->ast_node_value.assign.identifier = i;
	return node;
}

ast_node_t *ast_add_var_def(char *i, data_type_t i_type, ast_node_t *expression, bool non_modifiable)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = i_type;
	node->node_type = AST_VAR_DEF;
	node->ast_node_value.var_def.data_type = i_type;
	node->ast_node_value.var_def.expression = expression;
	node->ast_node_value.var_def.identifier = i;
	node->ast_node_value.var_def.non_modifiable = non_modifiable;
	return node;
}

ast_node_t *ast_add_return(ast_node_t *expression)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = expression->eval_type;
	node->node_type = AST_RETURN;
	node->ast_node_value.expression = expression;
	return node;
}

ast_node_t *ast_add_param(char *i, data_type_t i_type)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = i_type;
	node->node_type = AST_PARAM;
	node->ast_node_value.param.data_type = i_type;
	node->ast_node_value.param.identifier = i;
	return node;
}
