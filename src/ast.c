/**
 * @file ast.c
 * @brief Ast functions definition
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-12
 */

#include "ast.h"
#include <string.h>

/**
 * @brief Copies identifier to memory
 *
 * @param identifier Identifier to be copied
 * @return char* Pointer to the copy
 */
char *save_identifier(const char *identifier)
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
	node->func_def.unique_id = NULL;
	return node;
}

err_t ast_func_def_set_id(ast_node_t *func_def_node, const char *new_unique_id)
{
	if (func_def_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_func_def_set_id()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	char *id;
	id = save_identifier(new_unique_id);
	func_def_node->func_def.unique_id = id;
	return ERR_OK;
}

err_t ast_func_def_set_identificator(ast_node_t *func_def_node, const char *identificator)
{
	if (func_def_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_func_def_set_identificator()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	char *id;
	id = save_identifier(identificator);
	func_def_node->func_def.identificator = id;
	return ERR_OK;
}

err_t ast_func_def_set_block(ast_node_t *func_def_node, ast_node_t *block)
{
	if (func_def_node == NULL || block == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_func_def_set_block()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	func_def_node->func_def.block = block;
	return ERR_OK;
}

ast_node_t *ast_func_call_make(const char *unique_id)
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
	id = save_identifier(unique_id);
	node->func_call.unique_id = id;
	node->func_call.param_list_count = 0;
	node->func_call.param_lists = NULL;
	return node;
}

err_t ast_func_call_add_empty_param_list(ast_node_t *func_call_node)
{
	if (func_call_node == NULL || func_call_node->node_type != AST_FUNC_CALL)
	{
		print_err("Invalid function-call node in ast_func_call_add_empty_param_list().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	size_t list_count = func_call_node->func_call.param_list_count;
	param_node_list_t **new_lists =
		realloc(func_call_node->func_call.param_lists, (list_count + 1) * sizeof(param_node_list_t *));
	if (new_lists == NULL)
	{
		print_err("Failed to allocate memory in ast_func_call_add_empty_param_list().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	func_call_node->func_call.param_lists = new_lists;

	param_node_list_t *new_list = calloc(1, sizeof(param_node_list_t));
	if (new_list == NULL)
	{
		print_err("Failed to allocate memory in ast_func_call_add_empty_param_list().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	func_call_node->func_call.param_lists[list_count] = new_list;
	func_call_node->func_call.param_list_count++;
	return ERR_OK;
}

// TODO: Předělat (upravit)
err_t ast_func_call_add_param(ast_node_t *func_call_node, ast_node_t *param_node, size_t param_list_index)
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
	node->ext_def.unique_id = NULL;
	node->ext_def.block = NULL;
	return node;
}

err_t ast_ext_def_set_id(ast_node_t *func_def_node, const char *unique_id)
{
	if (func_def_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_ext_def_set_id()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	char *id;
	id = save_identifier(unique_id);
	func_def_node->ext_def.unique_id = id;
	return ERR_OK;
}

err_t ast_ext_def_set_identificator(ast_node_t *func_def_node, const char *identificator)
{
	if (func_def_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_ext_def_set_identificator()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	char *id;
	id = save_identifier(identificator);
	func_def_node->ext_def.identificator = id;
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

ast_node_t *ast_ext_call_make(const char *unique_id)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_EXT_CALL;

	char *id;
	id = save_identifier(unique_id);
	node->ext_call.unique_id = id;
	node->ext_call.param_lists = NULL;
	node->ext_call.param_list_count = 0;
	node->ext_call.base_node = NULL;
	return node;
}

// TODO: Dodelat
/*
err_t ast_ext_call_add_param(ast_node_t *ext_call_node, ast_node_t *param_node, unsigned param_list_index,
							 unsigned param_index)
{
}
*/

err_t ast_ext_call_add_base(ast_node_t *ext_call_node, ast_node_t *base_node)
{
	if (ext_call_node == NULL || base_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_ext_call_add_base()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	ext_call_node->ext_call.base_node = base_node;
	return ERR_OK;
}

ast_node_t *ast_block_make()
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_BLOCK;
	node->block.statements = NULL;
	node->block.statement_count = 0;
	return node;
}

err_t ast_block_add_statement(ast_node_t *block_node, ast_node_t *statement_node)
{
	if (block_node == NULL || statement_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_block_add_statement()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	ast_node_t **new_statement =
		realloc(block_node->block.statements, (block_node->block.statement_count + 1) * sizeof(ast_node_t *));

	if (new_statement == NULL)
	{
		print_err("Failed to allocate memory in ast_block_add_statement()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	block_node->block.statements = new_statement;
	block_node->block.statements[block_node->block.statement_count] = statement_node;
	block_node->block.statement_count++;
	return ERR_OK;
}

ast_node_t *ast_unary_op_make(operator_type_t operator, ast_node_t *operand)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	if (operator == OP_NEG || operator == OP_UMINUS || operator == OP_UPLUS)
	{
		node->eval_type = operand->eval_type;
		node->node_type = AST_UN_OPERATOR;
		node->un_operator.operator = operator;
		node->un_operator.operand = operand;
		return node;
	}
	else // Binary operations
	{
		free(node);
		return NULL;
	}
}

ast_node_t *ast_binary_op_make(operator_type_t operator, ast_node_t *operand_left, ast_node_t *operand_right)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	if (operator != OP_NEG && operator != OP_UMINUS && operator != OP_UPLUS)
	{
		if (operator == OP_MUL || operator == OP_DIV || operator == OP_MINUS || operator == OP_PLUS)
		{
			node->eval_type = operand_left->eval_type;
		}
		else // Type canges for comparing operations
		{
			node->eval_type = TYPE_BOOLEAN;
		}
		node->node_type = AST_BIN_OPERATOR;
		node->bin_operator.operand_left = operand_left;
		node->bin_operator.operand_right = operand_right;
		node->bin_operator.operator = operator;
		return node;
	}
	else // Unary operations
	{
		free(node);
		return NULL;
	}
}

ast_node_t *ast_var_def_make(const char *unique_id)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_VAR_DEF;

	char *id;
	id = save_identifier(unique_id);
	node->variable_definition.unique_id = id;
	node->variable_definition.assigned_value = NULL;
	return node;
}

err_t ast_var_def_set_assignment(ast_node_t *var_def_node, ast_node_t *assigned_value_node)
{
	if (var_def_node == NULL || assigned_value_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_var_def_set_assignment()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	var_def_node->variable_definition.assigned_value = assigned_value_node;
	return ERR_OK;
}

ast_node_t *ast_var_assign_make(const char *unique_id, ast_node_t *assigned_value)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_VAR_ASSIGN;

	char *id;
	id = save_identifier(unique_id);
	node->variable_assignment.unique_id = id;
	node->variable_assignment.assigned_value = assigned_value;
	return node;
}

ast_node_t *ast_var_use_make(const char *unique_id, data_type_t var_type)
{
	ast_node_t *leaf;
	leaf = malloc(sizeof(ast_node_t));

	if (leaf == NULL)
	{
		return NULL;
	}

	leaf->eval_type = var_type;
	leaf->node_type = AST_VAR_USE;

	char *id;
	id = save_identifier(unique_id);
	leaf->variable_use_unique_id = id;
	return leaf;
}

ast_node_t *ast_return_call_make(ast_node_t *return_value_node)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = return_value_node->eval_type;
	node->node_type = AST_RETURN_CALL;
	node->return_call = return_value_node;
	return node;
}

ast_node_t *ast_if_make(ast_node_t *condition_node)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_IF;
	node->if_statement.condition = condition_node;
	node->if_statement.if_block = NULL;
	node->if_statement.else_block = NULL;
	return node;
}

err_t ast_if_set_then_block(ast_node_t *if_node, ast_node_t *block)
{
	if (if_node == NULL || block == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_if_set_then_block()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if_node->if_statement.if_block = block;
	return ERR_OK;
}

err_t ast_if_set_else_block(ast_node_t *if_node, ast_node_t *block)
{
	if (if_node == NULL || block == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_if_set_else_block()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if_node->if_statement.else_block = block;
	return ERR_OK;
}

ast_node_t *ast_while_make(ast_node_t *condition)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_WHILE;
	node->while_loop.condition = condition;
	node->while_loop.block = NULL;
	return node;
}

err_t ast_while_set_block(ast_node_t *while_node, ast_node_t *block)
{
	if (while_node == NULL || block == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_while_set_block()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	while_node->while_loop.block = block;
	return ERR_OK;
}

ast_node_t *ast_for_make(const char *iterator_unique_id, ast_node_t *from_node)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_NONE;
	node->node_type = AST_FOR;
	node->for_loop.for_type = FOR_TO;

	char *id;
	id = save_identifier(iterator_unique_id);
	node->for_loop.iterator_unique_id = id;
	node->for_loop.from = from_node;
	node->for_loop.to = NULL;
	node->for_loop.until = NULL;
	node->for_loop.by = NULL;
	node->for_loop.guards = NULL;
	node->for_loop.guard_count = 0;
	node->for_loop.block = NULL;
	return node;
}

err_t ast_for_set_type(ast_node_t *for_node, for_type_t type)
{
	if (for_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_for_set_type()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	for_node->for_loop.for_type = type;
	return ERR_OK;
}

err_t ast_for_set_to(ast_node_t *for_node, ast_node_t *to_node)
{
	if (for_node == NULL || to_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_for_set_to()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	for_node->for_loop.to = to_node;
	return ERR_OK;
}

err_t ast_for_set_until(ast_node_t *for_node, ast_node_t *until_node)
{
	if (for_node == NULL || until_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_for_set_until()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	for_node->for_loop.until = until_node;
	return ERR_OK;
}

err_t ast_for_set_by(ast_node_t *for_node, ast_node_t *by_node)
{
	if (for_node == NULL || by_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_for_set_by()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	for_node->for_loop.by = by_node;
	return ERR_OK;
}

err_t ast_for_add_guard(ast_node_t *for_node, ast_node_t *guard_node)
{
	if (for_node == NULL || guard_node == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_for_add_guard()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	ast_node_t **new_guard =
		realloc(for_node->for_loop.guards, (for_node->for_loop.guard_count + 1) * sizeof(ast_node_t *));

	if (new_guard == NULL)
	{
		print_err("Failed to allocate memory in ast_for_add_guard()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	for_node->for_loop.guards = new_guard;
	for_node->for_loop.guards[for_node->for_loop.guard_count] = guard_node;
	for_node->for_loop.guard_count++;
	return ERR_OK;
}

err_t ast_for_set_block(ast_node_t *for_node, ast_node_t *block)
{
	if (for_node == NULL || block == NULL)
	{
		print_err("Tried to pass NULL ptr to ast_for_set_block()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	for_node->for_loop.block = block;
	return ERR_OK;
}

ast_node_t *ast_int2double_make(ast_node_t *operand)
{
	ast_node_t *node;
	node = malloc(sizeof(ast_node_t));

	if (node == NULL)
	{
		return NULL;
	}

	node->eval_type = TYPE_DOUBLE;
	node->node_type = AST_INT_TO_DOUBLE;
	node->int2double = operand;
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
	case AST_PROGRAM:
		for (size_t i = 0; i < root->program.func_def_count; i++)
		{
			destruct(root->program.func_defs[i]);
		}
		free(root->program.func_defs);

		for (size_t i = 0; i < root->program.ext_def_count; i++)
		{
			destruct(root->program.ext_defs[i]);
		}
		free(root->program.ext_defs);
		break;

	case AST_FUNC_DEF:
		destruct(root->func_def.block);
		free(root->func_def.unique_id);
		free(root->func_def.identificator);
		break;

	case AST_FUNC_CALL:
		for (size_t i = 0; i < root->func_call.param_list_count; i++)
		{
			for (size_t j = 0; j < root->func_call.param_lists[i]->param_count; j++)
			{
				destruct(root->func_call.param_lists[i]->param_node[j]);
			}
			free(root->func_call.param_lists[i]->param_node);
			free(root->func_call.param_lists[i]);
		}
		free(root->func_call.param_lists);
		free(root->func_call.unique_id);
		break;

	case AST_EXT_DEF:
		destruct(root->ext_def.block);
		free(root->ext_def.unique_id);
		free(root->ext_def.identificator);
		break;

	case AST_EXT_CALL:
		for (size_t i = 0; i < root->ext_call.param_list_count; i++)
		{
			for (size_t j = 0; j < root->ext_call.param_lists[i]->param_count; j++)
			{
				destruct(root->ext_call.param_lists[i]->param_node[j]);
			}
			free(root->ext_call.param_lists[i]->param_node);
			free(root->ext_call.param_lists[i]);
		}
		destruct(root->ext_call.base_node);
		free(root->ext_call.param_lists);
		free(root->ext_call.unique_id);
		break;

	case AST_BLOCK:
		for (size_t i = 0; i < root->block.statement_count; i++)
		{
			destruct(root->block.statements[i]);
		}
		free(root->block.statements);
		break;

	case AST_UN_OPERATOR:
		destruct(root->un_operator.operand);
		break;

	case AST_BIN_OPERATOR:
		destruct(root->bin_operator.operand_left);
		destruct(root->bin_operator.operand_right);
		break;

	case AST_VAR_DEF:
		destruct(root->variable_definition.assigned_value);
		free(root->variable_definition.unique_id);
		break;

	case AST_VAR_ASSIGN:
		destruct(root->variable_assignment.assigned_value);
		free(root->variable_assignment.unique_id);
		break;

	case AST_VAR_USE:
		free(root->variable_use_unique_id);
		break;

	case AST_RETURN_CALL:
		destruct(root->return_call);
		break;

	case AST_IF:
		destruct(root->if_statement.condition);
		destruct(root->if_statement.if_block);
		destruct(root->if_statement.else_block);
		break;

	case AST_WHILE:
		destruct(root->while_loop.condition);
		destruct(root->while_loop.block);
		break;

	case AST_FOR:
		for (size_t i = 0; i < root->for_loop.guard_count; i++)
		{
			destruct(root->for_loop.guards[i]);
		}
		free(root->for_loop.guards);

		destruct(root->for_loop.from);
		destruct(root->for_loop.to);
		destruct(root->for_loop.until);
		destruct(root->for_loop.by);
		destruct(root->for_loop.block);
		free(root->for_loop.iterator_unique_id);
		break;

	case AST_INT_TO_DOUBLE:
		destruct(root->int2double);
		break;

	// Leaf nodes
	default:
		break;
	}

	free(root);
	return;
}
