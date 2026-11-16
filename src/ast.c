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

	// Setting values
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
	if (!expression)
		return NULL;

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

ast_node_t *ast_add_while(ast_node_t *condition, ast_node_t *block)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = TYPE_NONE;
	node->node_type = AST_WHILE;
	node->ast_node_value.while_loop.block = block;
	node->ast_node_value.while_loop.condition = condition;
	return node;
}

ast_node_t *ast_add_if(ast_node_t *condition, ast_node_t *if_block, ast_node_t *else_block)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = TYPE_NONE;
	node->node_type = AST_IF;
	node->ast_node_value.if_statement.condition = condition;
	node->ast_node_value.if_statement.if_block = if_block;
	node->ast_node_value.if_statement.else_block = else_block;
	return node;
}

ast_node_t *ast_add_call(char *i, ast_node_t **args, size_t args_count)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = TYPE_NONE;
	node->node_type = AST_CALL;
	node->ast_node_value.call.arg_count = args_count;
	node->ast_node_value.call.arguments = args;
	node->ast_node_value.call.identifier = i;
	return node;
}

ast_node_t *ast_add_func_def(char *i, bool is_main, ast_node_t *block, ast_node_t **params, size_t param_count,
							 data_type_t return_type)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = TYPE_NONE;
	node->node_type = AST_FUNC_DEF;
	node->ast_node_value.func_def.block = block;
	node->ast_node_value.func_def.identifier = i;
	node->ast_node_value.func_def.is_main = is_main;
	node->ast_node_value.func_def.param_count = param_count;
	node->ast_node_value.func_def.params = params;
	node->ast_node_value.func_def.return_type = return_type;
	return node;
}

ast_node_t *ast_add_block(ast_node_t **expressions, size_t exp_count, bool is_root)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = TYPE_NONE;
	if (is_root)
	{
		node->node_type = AST_PROGRAM;
	}
	else
	{
		node->node_type = AST_BLOCK;
	}
	node->ast_node_value.code_block.expression = expressions;
	node->ast_node_value.code_block.exp_count = exp_count;
	return node;
}

/*
	AST_FUNC_DEF,
*/

void destruct(ast_node_t *root)
{
	if (root == NULL)
	{
		return;
	}

	node_type_t type = root->node_type;
	switch (type)
	{
	case (AST_RETURN):
		if (root->ast_node_value.expression == NULL)
		{
			free(root);
		}
		else
		{
			destruct(root->ast_node_value.expression);
		}
		break;

	case (AST_UN_OPERATOR):
		destruct(root->ast_node_value.un_operator.operand);
		break;

	case (AST_BIN_OPERATOR):
		destruct(root->ast_node_value.bin_operator.operand_left);
		destruct(root->ast_node_value.bin_operator.operand_right);
		break;

	case (AST_ASSIGN):
		destruct(root->ast_node_value.assign.expression);
		break;

	case (AST_VAR_DEF):
		destruct(root->ast_node_value.var_def.expression);
		break;

	case (AST_WHILE):
		destruct(root->ast_node_value.while_loop.block);
		destruct(root->ast_node_value.while_loop.condition);
		break;

	case (AST_IF):
		destruct(root->ast_node_value.if_statement.condition);
		destruct(root->ast_node_value.if_statement.else_block);
		destruct(root->ast_node_value.if_statement.if_block);
		break;

	case (AST_PROGRAM):
	case (AST_BLOCK):
		for (size_t i = 0; i < root->ast_node_value.code_block.exp_count; i++)
		{
			destruct(root->ast_node_value.code_block.expression[i]);
		}
		free(root->ast_node_value.code_block.expression);
		break;

	case (AST_CALL):
		for (size_t i = 0; i < root->ast_node_value.call.arg_count; i++)
		{
			destruct(root->ast_node_value.call.arguments[i]);
		}
		free(root->ast_node_value.call.arguments);
		break;

	case (AST_FUNC_DEF):
		destruct(root->ast_node_value.func_def.block);
		for (size_t i = 0; i < root->ast_node_value.func_def.param_count; i++)
		{
			destruct(root->ast_node_value.func_def.params[i]);
		}
		free(root->ast_node_value.func_def.params);
		break;

	// Leaf nodes
	default:
		free(root);
		break;
	}

	free(root);
	return;
}
