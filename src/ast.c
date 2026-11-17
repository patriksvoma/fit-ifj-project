/**
 * @file ast.c
 * @brief Ast functions definition
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-12
 */

#include "ast.h"
#include "error.h"

ast_node_t *ast_make_literal(literal_t *literal)
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

ast_node_t *ast_make_identifier(char *i, data_type_t i_type)
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

ast_node_t *ast_make_unary_op(ast_node_t *operand, operator_type_t op)
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

ast_node_t *ast_make_binary_op(ast_node_t *operand_left, ast_node_t *operand_right, operator_type_t op)
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

ast_node_t *ast_make_assign(char *i, data_type_t i_type, ast_node_t *expression)
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

ast_node_t *ast_make_var_def(char *i, data_type_t i_type, ast_node_t *expression, bool non_modifiable)
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

ast_node_t *ast_make_return(ast_node_t *expression)
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

ast_node_t *ast_make_param(char *i, data_type_t i_type)
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

ast_node_t *ast_make_while(ast_node_t *condition, ast_node_t *block)
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

ast_node_t *ast_make_if(ast_node_t *condition, ast_node_t *if_block, ast_node_t *else_block)
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

ast_node_t *ast_make_call(char *i)
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
	node->ast_node_value.call.identifier = i;
	node->ast_node_value.call.arg_count = 0;
	node->ast_node_value.call.arguments = NULL;
	return node;
}

ast_node_t *ast_make_func_def(bool is_main)
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
	node->ast_node_value.func_def.is_main = is_main;
	node->ast_node_value.func_def.param_count = 0;
	node->ast_node_value.func_def.params = NULL;
	node->ast_node_value.func_def.block = NULL;
	node->ast_node_value.func_def.return_type = TYPE_NONE;
	return node;
}

ast_node_t *ast_make_block()
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_BLOCK;
	node->ast_node_value.code_block.exp_count = 0;
	node->ast_node_value.code_block.expression = NULL;
	return node;
}

ast_node_t *ast_make_program()
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_PROGRAM;
	node->ast_node_value.code_block.exp_count = 0;
	node->ast_node_value.code_block.expression = NULL;
	return node;
}

void destruct(ast_node_t *root)
{
	if (root == NULL)
	{
		return;
	}

	// Recursively callíng the function until it comes into a leaf and then calling free
	node_type_t type = root->node_type;
	switch (type)
	{
	case AST_RETURN:
		destruct(root->ast_node_value.expression);
		break;

	case AST_UN_OPERATOR:
		destruct(root->ast_node_value.un_operator.operand);
		break;

	case AST_BIN_OPERATOR:
		destruct(root->ast_node_value.bin_operator.operand_left);
		destruct(root->ast_node_value.bin_operator.operand_right);
		break;

	case AST_ASSIGN:
		destruct(root->ast_node_value.assign.expression);
		break;

	case AST_VAR_DEF:
		destruct(root->ast_node_value.var_def.expression);
		break;

	case AST_WHILE:
		destruct(root->ast_node_value.while_loop.block);
		destruct(root->ast_node_value.while_loop.condition);
		break;

	case AST_IF:
		destruct(root->ast_node_value.if_statement.condition);
		destruct(root->ast_node_value.if_statement.else_block);
		destruct(root->ast_node_value.if_statement.if_block);
		break;

	case AST_PROGRAM:
	case AST_BLOCK:
		for (size_t i = 0; i < root->ast_node_value.code_block.exp_count; i++)
		{
			destruct(root->ast_node_value.code_block.expression[i]);
		}
		free(root->ast_node_value.code_block.expression);
		break;

	case AST_CALL:
		for (size_t i = 0; i < root->ast_node_value.call.arg_count; i++)
		{
			destruct(root->ast_node_value.call.arguments[i]);
		}
		free(root->ast_node_value.call.arguments);
		break;

	case AST_FUNC_DEF:
		destruct(root->ast_node_value.func_def.block);
		for (size_t i = 0; i < root->ast_node_value.func_def.param_count; i++)
		{
			destruct(root->ast_node_value.func_def.params[i]);
		}
		free(root->ast_node_value.func_def.params);
		break;

	case AST_BOOL_TO_INT:
		destruct(root->ast_node_value.operand);
		break;

	// Leaf nodes
	default:
		break;
	}

	free(root);
	return;
}

void ast_array_append(ast_node_t *node, ast_node_t *elem)
{
	ast_node_t **temp;

	if (!node || !elem)
		return;

	// Get the node type
	switch (node->node_type)
	{
	case AST_FUNC_DEF:
		if (node->ast_node_value.func_def.param_count == 0) // If the array is empty add first element with malloc
		{
			node->ast_node_value.func_def.params = malloc(sizeof(ast_node_t *));

			if (node->ast_node_value.func_def.params == NULL)
			{
				print_err("Failed to allocate memory in ast_array_append()", ERR_INTERNAL);
				return;
			}
		}
		else // Else use realloc to add room for one more element
		{
			temp = realloc(node->ast_node_value.func_def.params,
						   (node->ast_node_value.func_def.param_count + 1) * sizeof(ast_node_t *));

			if (temp == NULL)
			{
				free(node->ast_node_value.func_def.params);
				print_err("Failed to allocate memory in ast_array_append()", ERR_INTERNAL);
				return;
			}
			node->ast_node_value.func_def.params = temp;
		}

		// Saving pointer to the element at the end
		node->ast_node_value.func_def.params[node->ast_node_value.func_def.param_count] = elem;
		node->ast_node_value.func_def.param_count++;
		break;

	case AST_CALL:
		if (node->ast_node_value.call.arg_count == 0) // If the array is empty add first element with malloc
		{
			node->ast_node_value.call.arguments = malloc(sizeof(ast_node_t *));

			if (node->ast_node_value.call.arguments == NULL)
			{
				print_err("Failed to allocate memory in ast_array_append()", ERR_INTERNAL);
				return;
			}
		}

		else // Else use realloc to add room for one more element
		{
			temp = realloc(node->ast_node_value.call.arguments,
						   (node->ast_node_value.call.arg_count + 1) * sizeof(ast_node_t *));

			if (temp == NULL)
			{
				free(node->ast_node_value.call.arguments);
				print_err("Failed to allocate memory in ast_array_append()", ERR_INTERNAL);
				return;
			}
			node->ast_node_value.call.arguments = temp;
		}

		// Saving pointer to the element at the end
		node->ast_node_value.call.arguments[node->ast_node_value.call.arg_count] = elem;
		node->ast_node_value.call.arg_count++;
		break;

	case AST_BLOCK:
	case AST_PROGRAM:
		if (node->ast_node_value.code_block.exp_count == 0) // If the array is empty add first element with malloc
		{
			node->ast_node_value.code_block.expression = malloc(sizeof(ast_node_t *));

			if (node->ast_node_value.code_block.expression == NULL)
			{
				print_err("Failed to allocate memory in ast_array_append()", ERR_INTERNAL);
				return;
			}
		}
		else // Else use realloc to add room for one more element
		{
			temp = realloc(node->ast_node_value.code_block.expression,
						   (node->ast_node_value.code_block.exp_count + 1) * sizeof(ast_node_t *));

			if (temp == NULL)
			{
				free(node->ast_node_value.code_block.expression);
				print_err("Failed to allocate memory in ast_array_append()", ERR_INTERNAL);
				return;
			}
			node->ast_node_value.code_block.expression = temp;
		}

		// Saving pointer to the element at the end
		node->ast_node_value.code_block.expression[node->ast_node_value.code_block.exp_count] = elem;
		node->ast_node_value.code_block.exp_count++;
		break;

	// Wrong node type
	default:
		break;
	}
	return;
}
