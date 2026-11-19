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
#include <string.h>

/**
 * @brief Copies identifier to memory
 *
 * @param identifier Identifier to be copied
 * @return char* Pointer to the copy
 */
char *save_identifier(char *identifier)
{
	if (identifier == NULL)
	{
		return NULL;
	}

	char *ptr = malloc(strlen(identifier) + 1);

	if (ptr == NULL)
	{
		return NULL;
	}

	strcpy(ptr, identifier);
	return ptr;
}

ast_node_t *ast_literal_make(literal_t *literal)
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
		leaf->literal_int = literal->value.integer;
		break;

	case TYPE_DOUBLE:
		leaf->node_type = AST_LITERAL_DOUBLE;
		leaf->eval_type = literal->literal_type;
		leaf->literal_double = literal->value.decimal;
		break;

	case TYPE_STRING:
		leaf->node_type = AST_LITERAL_STRING;
		leaf->eval_type = literal->literal_type;
		leaf->literal_string = literal->value.string;
		break;

	case TYPE_BOOLEAN:
		leaf->node_type = AST_LITERAL_BOOLEAN;
		leaf->eval_type = literal->literal_type;
		leaf->literal_boolean = literal->value.boolean;
		break;

	default:
		leaf->node_type = AST_LITERAL_UNIT;
		leaf->eval_type = literal->literal_type;
		break;
	}

	return leaf;
}

ast_node_t *ast_program_make()
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_PROGRAM;
	node->program.func_def_count = 0;
	node->program.ext_def_count = 0;
	node->program.func_defs = NULL;
	node->program.ext_defs = NULL;
	return node;
}

err_t ast_program_add_func_def(ast_node_t *program_node, ast_node_t *func_def_node)
{
	if (program_node == NULL || func_def_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_program_add_func_def()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	ast_node_t **new_def =
		realloc(program_node->program.func_defs, (program_node->program.func_def_count + 1) * sizeof(ast_node_t *));

	if (new_def == NULL)
	{
		print_err("Failed to allocate memory in ast_program_add_func_def()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	program_node->program.func_defs = new_def;
	program_node->program.func_defs[program_node->program.func_def_count] = func_def_node;
	program_node->program.func_def_count++;
	return ERR_OK;
}

err_t ast_program_add_ext_def(ast_node_t *program_node, ast_node_t *ext_def_node)
{
	if (program_node == NULL || ext_def_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_program_add_ext_def()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	ast_node_t **new_def =
		realloc(program_node->program.ext_defs, (program_node->program.ext_def_count + 1) * sizeof(ast_node_t *));

	if (new_def == NULL)
	{
		print_err("Failed to allocate memory in ast_program_add_ext_def()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	program_node->program.ext_defs = new_def;
	program_node->program.ext_defs[program_node->program.ext_def_count] = ext_def_node;
	program_node->program.ext_def_count++;
	return ERR_OK;
}

ast_node_t *ast_func_def_make()
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_FUNC_DEF;
	node->func_def.block = NULL;
	node->func_def.identifier = NULL;
	return node;
}

err_t ast_func_def_set_id(ast_node_t *func_def_node, const char *new_id)
{
	if (func_def_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_func_def_set_id()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	char *id;
	id = save_identifier(new_id);
	func_def_node->func_def.identifier = id;
	return;
}

err_t ast_func_def_set_block(ast_node_t *func_def_node, ast_node_t *block)
{
	if (func_def_node == NULL || block == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_func_def_set_block()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	func_def_node->func_def.block = block;
	return;
}

ast_node_t *ast_func_call_make(char *identificator)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_FUNC_CALL;

	char *id;
	id = save_identifier(identificator);
	node->func_call.identifier = id;
	node->func_call.param_list_count = 0;
	node->func_call.param_lists = 0;
	return node;
}

// TODO: Předělat (upravit)
err_t ast_func_call_add_param(ast_node_t *func_call_node, ast_node_t *param_node, unsigned param_list_index)
{
	if (func_call_node == NULL || param_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_func_call_add_param()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// List wasn't initilized yet
	if (func_call_node->func_call.param_list_count <= param_list_index)
	{
		// Allocating memory for all lists
		param_node_list_t **new_list =
			realloc(func_call_node->func_call.param_lists, (param_list_index + 1) * sizeof(param_node_list_t *));

		if (new_list == NULL)
		{
			print_err("Failed to allocate memory in ast_func_call_add_param()", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		func_call_node->func_call.param_lists = new_list;

		// Setting skiped lists to NULL
		for (unsigned i = func_call_node->func_call.param_list_count; i <= param_list_index; i++)
		{
			param_node_list_t *list = calloc(1, sizeof(param_node_list_t));

			if (list == NULL)
			{
				print_err("Failed to allocate memory in ast_func_call_add_param()", ERR_INTERNAL);
				return ERR_INTERNAL;
			}

			func_call_node->func_call.param_lists[i] = list;
		}

		// Allocating memory for new node
		func_call_node->func_call.param_lists[param_list_index]->param_node = malloc(sizeof(ast_node_t *));

		if (func_call_node->func_call.param_lists[param_list_index]->param_node == NULL)
		{
			print_err("Failed to allocate memory in ast_func_call_add_param()", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		func_call_node->func_call.param_lists[param_list_index]->param_node[0] = param_node;
		func_call_node->func_call.param_lists[param_list_index]->param_count = 1;
		func_call_node->func_call.param_list_count = param_list_index + 1;
		return ERR_OK;
	}
	// List already exists
	else
	{
		// Realocating memory for new node
		ast_node_t **new_node =
			realloc(func_call_node->func_call.param_lists[param_list_index]->param_node,
					(func_call_node->func_call.param_lists[param_list_index]->param_count + 1) * sizeof(ast_node_t *));

		if (new_node == NULL)
		{
			print_err("Failed to allocate memory in ast_func_call_add_param()", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		func_call_node->func_call.param_lists[param_list_index]->param_node = new_node;
		func_call_node->func_call.param_lists[param_list_index]
			->param_node[func_call_node->func_call.param_lists[param_list_index]->param_count] = param_node;
		func_call_node->func_call.param_lists[param_list_index]->param_count++;
		return ERR_OK;
	}
}

ast_node_t *ast_ext_def_make(data_type_t extended_type)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = extended_type;
	node->node_type = AST_EXT_DEF;
	node->ext_def.identifier = NULL;
	node->ext_def.block = NULL;
	return node;
}

err_t ast_ext_def_set_id(ast_node_t *func_def_node, const char *newId)
{
	if (func_def_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_ext_def_set_id()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	char *id;
	id = save_identifier;
	func_def_node->ext_def.identifier = id;
	return ERR_OK;
}

err_t ast_ext_def_set_block(ast_node_t *func_def_node, ast_node_t *block)
{
	if (func_def_node == NULL || block == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_ext_def_set_block()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	func_def_node->ext_def.block = block;
	return ERR_OK;
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

		if (op == OP_CONCATENATION || op == OP_MUL || op == OP_DIV || op == OP_MINUS || op == OP_PLUS)
		{
			node->eval_type = operand_left->eval_type;
		}
		else // Type canges for comparing operations
		{
			node->eval_type = TYPE_BOOLEAN;
		}
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

ast_node_t *ast_make_int_to_double(ast_node_t *operand)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = TYPE_DOUBLE;
	node->node_type = AST_INT_TO_DOUBLE;
	node->ast_node_value.operand = operand;
	return node;
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

// TODO: Tady to taky bude potřeba popřípadě dodělat
ast_node_t *ast_make_extension_def()
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	// Setting values
	node->eval_type = TYPE_NONE;
	node->node_type = AST_EXTENSION_DEF;
	node->ast_node_value.extension_def.param_count = 0;
	node->ast_node_value.extension_def.params = NULL;
	node->ast_node_value.extension_def.block = NULL;
	node->ast_node_value.extension_def.return_type = TYPE_NONE;
	node->ast_node_value.extension_def.base_type = TYPE_NONE;
	return node;
}

ast_node_t *ast_make_extension_use(ast_node_t *base)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_EXTENSION_USE;
	node->ast_node_value.base = base;
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

	case AST_EXTENSION_DEF:
		destruct(root->ast_node_value.extension_def.block);
		for (size_t i = 0; i < root->ast_node_value.extension_def.param_count; i++)
		{
			destruct(root->ast_node_value.extension_def.params[i]);
		}
		free(root->ast_node_value.extension_def.params);
		break;

	case AST_INT_TO_DOUBLE:
		destruct(root->ast_node_value.operand);
		break;

	case AST_EXTENSION_USE:
		destruct(root->ast_node_value.base);
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
				print_err("Failed to allocate memory in ast_array_append()", ERR_INTERNAL);
				return;
			}
			node->ast_node_value.call.arguments = temp;
		}

		// Saving pointer to the element at the end
		node->ast_node_value.call.arguments[node->ast_node_value.call.arg_count] = elem;
		node->ast_node_value.call.arg_count++;
		break;

	case AST_EXTENSION_DEF:
		if (node->ast_node_value.extension_def.param_count == 0) // If the array is empty add first element with malloc
		{
			node->ast_node_value.extension_def.params = malloc(sizeof(ast_node_t *));

			if (node->ast_node_value.extension_def.params == NULL)
			{
				print_err("Failed to allocate memory in ast_array_append()", ERR_INTERNAL);
				return;
			}
		}

		else // Else use realloc to add room for one more element
		{
			temp = realloc(node->ast_node_value.extension_def.params,
						   (node->ast_node_value.extension_def.param_count + 1) * sizeof(ast_node_t *));

			if (temp == NULL)
			{
				print_err("Failed to allocate memory in ast_array_append()", ERR_INTERNAL);
				return;
			}
			node->ast_node_value.extension_def.params = temp;
		}

		// Saving pointer to the element at the end
		node->ast_node_value.extension_def.params[node->ast_node_value.extension_def.param_count] = elem;
		node->ast_node_value.extension_def.param_count++;
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
