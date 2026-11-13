/**
 * @file ast.c
 * @brief Ast functions definition
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-12
 */

#include "ast.h"

ast_node_t *ast_add_literal(literal_t *literal)
{
	ast_node_t *leaf;
	leaf = malloc(sizeof(ast_node_t));

	if (leaf == NULL)
	{
		return NULL;
	}

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
