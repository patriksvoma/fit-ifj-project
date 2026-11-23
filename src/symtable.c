/**
 * @file symtable.c
 * @brief Symbol table for the IFJ26 compiler
 *
 * @author Jakub Kvasnička (xkvasnj00@stud.fit.vut.cz)
 *
 * @date 2026-11-13
 */

#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates a new symbol tree node
 *
 * @param key Identifier of the new node
 * @param data Data that are given to the new node
 * @return tree_node_t Pointer to created node, NULL if allocation fails or NUll key
 */
tree_node_t *symtable_create_node(const char *key, symbol_data_t data)
{
	if (!key)
		return NULL;

	tree_node_t *new = malloc(sizeof(tree_node_t));

	if (!new)
		return NULL;

	new->key = malloc(strlen(key) + 1);

	if (!new->key)
	{
		free(new);
		return NULL;
	}

	strcpy(new->key, key);

	new->data = data;
	new->height = 1;
	new->left = NULL;
	new->right = NULL;
	return new;
}

symbol_data_t *symtable_item_find_local(symtable_t *table, const char *key)
{
	if (key == NULL || table == NULL || table->root_node == NULL)
		return NULL;

	if (strcmp(key, table->root_node->key) == 0)
	{
		return &table->root_node->data;
	}
	else if (strcmp(key, table->root_node->key) < 0)
	{
		symtable_t left_table = *table;
		left_table.root_node = table->root_node->left;
		return symtable_item_find_local(&left_table, key);
	}
	else
	{
		symtable_t right_table = *table;
		right_table.root_node = table->root_node->right;
		return symtable_item_find_local(&right_table, key);
	}
}

symbol_data_t *symtable_item_find(symtable_t *table, const char *key)
{
	if (key == NULL || table == NULL)
		return NULL;

	if (symtable_item_find_local(table, key) != NULL)
		return symtable_item_find_local(table, key);

	if (table->parent != NULL)
		return symtable_item_find(table->parent, key);

	return NULL;
}

bool symtable_item_exists(symtable_t *table, const char *key)
{
	if (table == NULL || key == NULL)
		return false;

	if (symtable_item_find_local(table, key))
		return true;

	return false;
}

int height(const tree_node_t *node)
{
	if (node == NULL)
		return 0;

	return node->height;
}

int max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

/**
 * @brief Get balance factor of a node
 *
 * @param node Pointer to the node whose balance is calculated
 * @return int Balance factor of the node, or 0 in case of nonexisting node
 */
int get_balance(const tree_node_t *node)
{
	if (node == NULL)
		return 0;

	return height(node->left) - height(node->right);
}

tree_node_t *rotate_right(tree_node_t *root)
{
	tree_node_t *child = root->left;
	root->left = child->right;
	child->right = root;

	root->height = max(height(root->left), height(root->right)) + 1;
	child->height = max(height(child->left), height(child->right)) + 1;

	return child;
}

tree_node_t *rotate_left(tree_node_t *root)
{
	tree_node_t *child = root->right;
	root->right = child->left;
	child->left = root;

	root->height = max(height(root->left), height(root->right)) + 1;
	child->height = max(height(child->left), height(child->right)) + 1;

	return child;
}

/**
 * @brief Inserts a new node into a tree, if the tree is empty creates a root
 * 			if it is not empty inserts the node based on its key and then updates height and gets balance
 * 			after that it rebalances the tree
 *
 * @param root The root of tree into which the node is inserted
 * @param key Identifier of the inserted node
 * @param data Data to add to the inserted node
 * @param allocation_fail If allocation fails has value true, false otherwise
 * @return tree_node_t Returns pointer to new root of the tree, or NULL in case of error
 */
tree_node_t *insert_node(tree_node_t *root, const char *key, symbol_data_t data, bool *allocation_fail)
{
	if (root == NULL)
	{
		tree_node_t *new_node = symtable_create_node(key, data);

		if (new_node == NULL)
		{
			*allocation_fail = true;
			return NULL;
		}
		return new_node;
	}

	if (strcmp(key, root->key) == 0)
		return root;

	if (strcmp(key, root->key) < 0)
		root->left = insert_node(root->left, key, data, allocation_fail);
	else
		root->right = insert_node(root->right, key, data, allocation_fail);

	if (*allocation_fail)
		return root;

	root->height = max(height(root->left), height(root->right)) + 1;
	int balance = get_balance(root);

	if (balance > 1 && strcmp(key, root->left->key) < 0) // LL
	{
		return rotate_right(root);
	}
	else if (balance < -1 && strcmp(key, root->right->key) > 0) // RR
	{
		return rotate_left(root);
	}
	else if (balance > 1 && strcmp(key, root->left->key) > 0) // LR
	{
		root->left = rotate_left(root->left);
		return rotate_right(root);
	}
	else if (balance < -1 && strcmp(key, root->right->key) < 0) // RL
	{
		root->right = rotate_right(root->right);
		return rotate_left(root);
	}
	return root;
}

/**
 * @brief Creates new symbol in symbol table
 *
 * @param table Pointer to the symbol table into which symbol is created
 * @param key Identifier of new symbol
 * @return symbol_data_t Pointer to data of the new symbol, NULL if symbol already exists or in case of error
 */
symbol_data_t *symtable_item_create(symtable_t *table, const char *key)
{
	if (table == NULL || key == NULL)
		return NULL;

	if (symtable_item_exists(table, key))
		return NULL;

	symbol_data_t data = {0};
	bool allocation_fail = false;

	table->root_node = insert_node(table->root_node, key, data, &allocation_fail);

	if (allocation_fail)
		return NULL;

	return symtable_item_find_local(table, key);
}

bool symtable_item_defined_local(symtable_t *table, const char *key)
{
	if (table == NULL || key == NULL)
		return false;

	symbol_data_t *data = symtable_item_find_local(table, key);
	if (!data)
		return false;

	if (data->symbol_type == SYM_VARIABLE)
	{
		return data->data.variable.is_defined;
	}
	else if (data->symbol_type == SYM_FUNCTION)
	{
		return data->data.function.is_defined;
	}
	else if (data->symbol_type == SYM_EXTENSION)
	{
		return data->data.extension.is_defined;
	}
	return false;
}

bool symtable_item_defined(symtable_t *table, const char *key)
{
	if (table == NULL || key == NULL)
		return false;

	symbol_data_t *data = symtable_item_find(table, key);
	if (!data)
		return false;

	if (data->symbol_type == SYM_VARIABLE)
	{
		return data->data.variable.is_defined;
	}
	else if (data->symbol_type == SYM_FUNCTION)
	{
		return data->data.function.is_defined;
	}
	else if (data->symbol_type == SYM_EXTENSION)
	{
		return data->data.extension.is_defined;
	}
	return false;
}

err_t symbol_add_param_list(symbol_data_t *symbol)
{
	if (symbol == NULL)
	{
		print_err("NULL pointer in symbol_add_param_list().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	param_list_t *new_list = malloc(sizeof(param_list_t));

	if (new_list == NULL)
	{
		print_err("Failed to allocate memory in symbol_add_param_list().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	new_list->params = NULL;
	new_list->param_count = 0;

	if (symbol->symbol_type == SYM_FUNCTION)
	{
		param_list_t **new_lists = realloc(symbol->data.function.param_lists,
										   (symbol->data.function.param_list_count + 1) * sizeof(param_list_t *));

		if (new_lists == NULL)
		{
			free(new_list);
			print_err("Failed to allocate memory in symbol_add_param_list().", ERR_INTERNAL);
			return ERR_INTERNAL;
		}
		symbol->data.function.param_lists = new_lists;
		symbol->data.function.param_lists[symbol->data.function.param_list_count] = new_list;
		symbol->data.function.param_list_count++;
		return ERR_OK;
	}
	else if (symbol->symbol_type == SYM_EXTENSION)
	{
		param_list_t **new_lists = realloc(symbol->data.extension.param_lists,
										   (symbol->data.extension.param_list_count + 1) * sizeof(param_list_t *));

		if (new_lists == NULL)
		{
			free(new_list);
			print_err("Failed to allocate memory in symbol_add_param_list().", ERR_INTERNAL);
			return ERR_INTERNAL;
		}
		symbol->data.extension.param_lists = new_lists;
		symbol->data.extension.param_lists[symbol->data.extension.param_list_count] = new_list;
		symbol->data.extension.param_list_count++;
		return ERR_OK;
	}

	free(new_list);
	print_err("Wrong symbol type in symbol_add_param_list()", ERR_INTERNAL);
	return ERR_INTERNAL;
}

err_t symbol_add_param(symbol_data_t *symbol)
{
	if (symbol == NULL)
	{
		print_err("NULL pointer in symbol_add_param().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (symbol->symbol_type == SYM_FUNCTION)
	{
		if (symbol->data.function.param_list_count == 0)
		{
			return ERR_INTERNAL;
		}

		param_list_t *last_list = symbol->data.function.param_lists[symbol->data.function.param_list_count - 1];

		param_t *new_param = malloc(sizeof(param_t));

		if (new_param == NULL)
		{
			print_err("Failed to allocate memory in symbol_add_param().", ERR_INTERNAL);
			return ERR_INTERNAL;
		}
		new_param->identifier = NULL;
		new_param->type = 0;

		param_t **new_params = realloc(last_list->params, (last_list->param_count + 1) * sizeof(param_t *));

		if (new_params == NULL)
		{
			free(new_param);
			print_err("Failed to allocate memory in symbol_add_param().", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		last_list->params = new_params;

		last_list->params[last_list->param_count] = new_param;
		last_list->param_count++;
		return ERR_OK;
	}
	else if (symbol->symbol_type == SYM_EXTENSION)
	{
		if (symbol->data.extension.param_list_count == 0)
		{
			return ERR_INTERNAL;
		}

		param_list_t *last_list = symbol->data.extension.param_lists[symbol->data.extension.param_list_count - 1];
		param_t *new_param = malloc(sizeof(param_t));

		if (new_param == NULL)
		{
			print_err("Failed to allocate memory in symbol_add_param().", ERR_INTERNAL);
			return ERR_INTERNAL;
		}
		new_param->identifier = NULL;
		new_param->type = 0;

		param_t **new_params = realloc(last_list->params, (last_list->param_count + 1) * sizeof(param_t *));

		if (new_params == NULL)
		{
			free(new_param);
			print_err("Failed to allocate memory in symbol_add_param().", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		last_list->params = new_params;

		last_list->params[last_list->param_count] = new_param;
		last_list->param_count++;
		return ERR_OK;
	}
	print_err("Wrong symbol type in symbol_add_param_list()", ERR_INTERNAL);
	return ERR_INTERNAL;
}

param_t *symbol_top_param(symbol_data_t *symbol)
{
	if (symbol == NULL)
		return NULL;

	if (symbol->symbol_type == SYM_FUNCTION)
	{
		if (symbol->data.function.param_list_count == 0)
			return NULL;

		param_list_t *last_list = symbol->data.function.param_lists[symbol->data.function.param_list_count - 1];

		if (last_list == NULL || last_list->param_count == 0)
			return NULL;

		return last_list->params[last_list->param_count - 1];
	}
	else if (symbol->symbol_type == SYM_EXTENSION)
	{
		if (symbol->data.extension.param_list_count == 0)
			return NULL;

		param_list_t *last_list = symbol->data.extension.param_lists[symbol->data.extension.param_list_count - 1];

		if (last_list == NULL || last_list->param_count == 0)
			return NULL;

		return last_list->params[last_list->param_count - 1];
	}
	return NULL;
}

/**
 * @brief Recursively destroys node and its subtrees, frees allocated data of the node
 *
 * @param node Pointer to node that will be destroyed
 */
void destroy_tree(tree_node_t *node)
{
	if (node == NULL)
		return;

	if (node->left)
	{
		destroy_tree(node->left);
	}

	if (node->right)
	{
		destroy_tree(node->right);
	}

	if (node->data.symbol_type == SYM_VARIABLE)
	{
		free(node->data.data.variable.unique_name);
	}
	else if (node->data.symbol_type == SYM_FUNCTION)
	{
		free(node->data.data.function.unique_name);
		for (size_t i = 0; i < node->data.data.function.param_list_count; i++)
		{
			param_list_t *list = node->data.data.function.param_lists[i];

			for (size_t j = 0; j < list->param_count; j++)
			{
				free(list->params[j]->identifier);
				free(list->params[j]);
			}
			free(list->params);
			free(list);
		}
		free(node->data.data.function.param_lists);
	}
	else if (node->data.symbol_type == SYM_EXTENSION)
	{
		for (size_t i = 0; i < node->data.data.extension.param_list_count; i++)
		{
			param_list_t *list = node->data.data.extension.param_lists[i];

			for (size_t j = 0; j < list->param_count; j++)
			{
				free(list->params[j]->identifier);
				free(list->params[j]);
			}
			free(list->params);
			free(list);
		}
		free(node->data.data.extension.param_lists);
		free(node->data.data.extension.base_param.identifier);
	}
	free(node->key);
	free(node);
}

void symtable_destroy(symtable_t **table)
{
	if (table == NULL || *table == NULL)
		return;

	destroy_tree((*table)->root_node);
	free(*table);
	*table = NULL;
}

symtable_t *local_table_create(symtable_t *parent)
{
	symtable_t *table = malloc(sizeof(symtable_t));
	if (!table)
		return NULL;

	table->parent = parent;
	table->root_node = NULL;
	table->child_table_counter = 0;

	// Increase the parent nodes child counter
	// Used for generating unique identifiers
	if (parent)
		parent->child_table_counter++;

	return table;
}

symtable_t *local_table_destroy(symtable_t *table)
{
	if (!table)
		return NULL;

	symtable_t *parent = table->parent;

	symtable_destroy(&table);

	return parent;
}

err_t append_last_table_counter(char **buf, size_t *buf_len, symtable_t *table)
{
	// Go recuresively to the global table, as that is supposed to be the first one to be appended
	if (!table)
		return ERR_OK;
	err_t result = append_last_table_counter(buf, buf_len, table->parent);
	if (result)
		return result;

	// Create a string where the table counter will be written
	char counter_str[12] = {0};

	// Print the string and count the length
	sprintf(counter_str, "$%u", table->child_table_counter);
	size_t len = strlen(counter_str);

	// Reallocate the buffer with the size increased by len, then append the counter string
	char *new_buf = realloc(*buf, *buf_len + len);
	if (!new_buf)
	{
		free(*buf);
		print_err("Failed to reallocate memory in append_last_table_counter()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	*buf = new_buf;

	memcpy(*buf + *buf_len - 1, counter_str, len);
	*buf_len += len;
	(*buf)[*buf_len - 1] = '\0';

	return ERR_OK;
}

err_t symbol_generate_unique_id(const char *identifier, symbol_data_t *symbol, const symtable_t *table)
{
	if (!identifier || !symbol || !table)
	{
		print_err("Null pointer reference in symbol_generate_unique_id().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (symbol->symbol_type == SYM_EXTENSION)
	{
		print_err("Do not use symbol_generate_unique_id() for extensions. Use generate_unique_ext_id()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	size_t buf_len = 1;
	char *buf = malloc(buf_len);
	if (!buf)
	{
		print_err("Failed to allocate a buffer in symbol_generate_unique_id()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Append all the counter strings ($1$4$2)
	err_t result = append_last_table_counter(&buf, &buf_len, table->parent);
	if (result)
		return result;

	// Append the variable name
	size_t id_len = strlen(identifier);
	char *new_buf = realloc(buf, buf_len + id_len);
	if (!new_buf)
	{
		free(buf);
		print_err("Failed to reallocate a buffer in symbol_generate_unique_id()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	buf = new_buf;

	memcpy(buf + buf_len - 1, identifier, id_len);
	buf_len += id_len;
	buf[buf_len - 1] = '\0';

	switch (symbol->symbol_type)
	{
	case SYM_VARIABLE:
		symbol->data.variable.unique_name = buf;
		break;
	case SYM_FUNCTION:
		symbol->data.function.unique_name = buf;
		break;
	case SYM_EXTENSION:
		return ERR_INTERNAL;
	}
	return ERR_OK;
}

err_t symbol_add_parameters_into_table(symbol_data_t *symbol, symtable_t *table)
{
	if (!symbol || !table)
	{
		print_err("Null-pointer exception in symbol_func_add_parameters_into_table()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	err_t result;

	if (symbol->symbol_type == SYM_VARIABLE)
	{
		print_err("symbol_func_add_parameters_into_table() called on a variable!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	else if (symbol->symbol_type == SYM_FUNCTION)
	{
		for (unsigned list_index = 0; list_index < symbol->data.function.param_list_count; list_index++)
		{
			param_list_t *list = symbol->data.function.param_lists[list_index];
			if (list == NULL)
			{
				print_err("Trying to access a NULL list inside symbol_func_add_parameters_into_table()!", ERR_INTERNAL);
				return ERR_INTERNAL;
			}

			for (unsigned param_index = 0; param_index < list->param_count; param_index++)
			{
				const param_t *param = list->params[param_index];
				if (param == NULL)
				{
					print_err("Trying to access NULL param inside symbol_func_add_parameters_into_table()!",
							  ERR_INTERNAL);
					return ERR_INTERNAL;
				}

				// Check for redefinitions
				if (symtable_item_defined_local(table, param->identifier))
				{
					print_err("Multiple parameters of the same name are not allowed.", ERR_SEMANTIC_REDEFINITION);
					return ERR_SEMANTIC_REDEFINITION;
				}

				symbol_data_t *entry = symtable_item_create(table, param->identifier);
				if (!entry)
				{
					print_err("Failed to create a symbol table entry in symbol_func_add_parameters_into_table()!",
							  ERR_INTERNAL);
					return ERR_INTERNAL;
				}
				entry->symbol_type = SYM_VARIABLE;
				entry->data.variable.is_defined = true;
				entry->data.variable.mutable = false;
				entry->data.variable.type = param->type;
				result = symbol_generate_unique_id(param->identifier, entry, table);
				if (result)
					return result;
			}
		}
	}
	else if (symbol->symbol_type == SYM_EXTENSION)
	{
		for (unsigned list_index = 0; list_index < symbol->data.extension.param_list_count; list_index++)
		{
			param_list_t *list = symbol->data.extension.param_lists[list_index];
			if (list == NULL)
			{
				print_err("Trying to access a NULL list inside symbol_func_add_parameters_into_table()!", ERR_INTERNAL);
				return ERR_INTERNAL;
			}

			for (unsigned param_index = 0; param_index < list->param_count; param_index++)
			{
				const param_t *param = list->params[param_index];
				if (param == NULL)
				{
					print_err("Trying to access NULL param inside symbol_func_add_parameters_into_table()!",
							  ERR_INTERNAL);
					return ERR_INTERNAL;
				}

				// Check for redefinitions
				if (symtable_item_defined_local(table, param->identifier))
				{
					print_err("Multiple parameters of the same name are not allowed.", ERR_SEMANTIC_REDEFINITION);
					return ERR_SEMANTIC_REDEFINITION;
				}

				symbol_data_t *entry = symtable_item_create(table, param->identifier);
				if (!entry)
				{
					print_err("Failed to create a symbol table entry in symbol_func_add_parameters_into_table()!",
							  ERR_INTERNAL);
					return ERR_INTERNAL;
				}
				entry->symbol_type = SYM_VARIABLE;
				entry->data.variable.is_defined = true;
				entry->data.variable.mutable = false;
				entry->data.variable.type = param->type;
				result = symbol_generate_unique_id(param->identifier, entry, table);
				if (result)
					return result;
			}
		}

		// For extensions, also add the base parameter as a variable
		const param_t *base_param = &symbol->data.extension.base_param;

		// Check for redefinitions first
		if (symtable_item_defined_local(table, base_param->identifier))
		{
			print_err("A parameter of an extension method must not be of the same name as the extension base.",
					  ERR_SEMANTIC_REDEFINITION);
			return ERR_SEMANTIC_REDEFINITION;
		}

		symbol_data_t *entry = symtable_item_create(table, base_param->identifier);
		if (!entry)
		{
			print_err("Failed to create a symbol table entry in symbol_func_add_parameters_into_table()!",
					  ERR_INTERNAL);
			return ERR_INTERNAL;
		}
		entry->symbol_type = SYM_VARIABLE;
		entry->data.variable.is_defined = true;
		entry->data.variable.mutable = false;
		entry->data.variable.type = base_param->type;
		result = symbol_generate_unique_id(base_param->identifier, entry, table);
		if (result)
			return result;
	}
	else
	{
		return ERR_INTERNAL;
	}

	return ERR_OK;
}

err_t symbol_add_param_named(symbol_data_t *symbol, const char *param_name, data_type_t param_type)
{
	err_t result;

	result = symbol_add_param(symbol);
	if (result)
		return result;

	param_t *param = symbol_top_param(symbol);
	if (!param)
	{
		print_err("Null-pointer reference in symbol_add_param_named().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	param->type = param_type;
	param->identifier = malloc(strlen(param_name) + 1);
	if (!param->identifier)
	{
		print_err("Failed to allocate memory in symbol_add_param_named().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	return ERR_OK;
}

/**
 * @brief Creates an allocated identifier prefixed by "type$"
 *
 * @param identifer Original identifier
 * @param type Extension base data type
 * @return char* Address to the allocated string
 */
char *generate_unique_ext_id(const char *identifer, const data_type_t type)
{
	char *result;
	char *prefix;

	switch (type)
	{
	case TYPE_INT:
		prefix = "Int$";
		break;
	case TYPE_DOUBLE:
		prefix = "Double$";
		break;
	case TYPE_STRING:
		prefix = "String$";
		break;
	case TYPE_UNIT:
		prefix = "Unit$";
		break;
	case TYPE_BOOLEAN:
		prefix = "Boolean$";
		break;
	case TYPE_ANY:
		prefix = "Any$";
		break;
	default:
		return NULL;
	}

	result = malloc(strlen(identifer) + strlen(prefix) + 1);
	if (!result)
	{
		print_err("Failed to allocate memory in generate_unique_ext_id().", ERR_INTERNAL);
		return NULL;
	}

	strcpy(result, prefix);
	strcpy(result + strlen(prefix), identifer);

	return result;
}

err_t symtable_insert_builtins(symtable_t *func_table, symtable_t *ext_table)
{
	if (!func_table || !ext_table)
	{
		print_err("Null-pointer reference in symtable_insert_builtins().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	symbol_data_t *entry;
	err_t result;

	// BASE

	// print(value: Any): Unit
	entry = symtable_item_create(func_table, "print");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for print().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_UNIT;
	result = symbol_generate_unique_id("print", entry, func_table);
	if (result)
		return result;
	result = symbol_add_param_list(entry);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "value", TYPE_ANY);
	if (result)
		return result;

	// readLine(): String
	entry = symtable_item_create(func_table, "readLine");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for readLine().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_STRING;
	result = symbol_generate_unique_id("readLine", entry, func_table);
	if (result)
		return result;

	// readInt(): Int
	entry = symtable_item_create(func_table, "readInt");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for readInt().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_INT;
	result = symbol_generate_unique_id("readInt", entry, func_table);
	if (result)
		return result;

	// readDouble(): Double
	entry = symtable_item_create(func_table, "readDouble");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for readDouble().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_DOUBLE;
	result = symbol_generate_unique_id("readDouble", entry, func_table);
	if (result)
		return result;

	// toStr(value: Any): String
	entry = symtable_item_create(func_table, "toStr");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for toStr().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_STRING;
	result = symbol_generate_unique_id("toStr", entry, func_table);
	if (result)
		return result;
	result = symbol_add_param_list(entry);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "value", TYPE_ANY);
	if (result)
		return result;

	// toInt(value: Double): Int
	entry = symtable_item_create(func_table, "toInt");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for toInt().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_INT;
	result = symbol_generate_unique_id("toInt", entry, func_table);
	if (result)
		return result;
	result = symbol_add_param_list(entry);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "value", TYPE_DOUBLE);
	if (result)
		return result;

	// strLength(value: String): Int
	entry = symtable_item_create(func_table, "strLength");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for strLength().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_INT;
	result = symbol_generate_unique_id("strLength", entry, func_table);
	if (result)
		return result;
	result = symbol_add_param_list(entry);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "value", TYPE_STRING);
	if (result)
		return result;

	// subStr(value: String, start: Int, end: Int): String
	entry = symtable_item_create(func_table, "subStr");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for subStr().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_STRING;
	result = symbol_generate_unique_id("subStr", entry, func_table);
	if (result)
		return result;
	result = symbol_add_param_list(entry);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "value", TYPE_STRING);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "start", TYPE_INT);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "end", TYPE_INT);
	if (result)
		return result;

	// ord(value: String, index: Int): Int
	entry = symtable_item_create(func_table, "ord");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for ord().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_INT;
	result = symbol_generate_unique_id("ord", entry, func_table);
	if (result)
		return result;
	result = symbol_add_param_list(entry);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "value", TYPE_STRING);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "index", TYPE_INT);
	if (result)
		return result;

	// chr(code: Int): String
	entry = symtable_item_create(func_table, "chr");
	if (!entry)
	{
		print_err("Faild to create a symbol table entry for chr().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	entry->symbol_type = SYM_FUNCTION;
	entry->data.function.is_defined = true;
	entry->data.function.is_main = false;
	entry->data.function.return_type = TYPE_STRING;
	result = symbol_generate_unique_id("chr", entry, func_table);
	if (result)
		return result;
	result = symbol_add_param_list(entry);
	if (result)
		return result;
	result = symbol_add_param_named(entry, "code", TYPE_INT);
	if (result)
		return result;

	// VALMEM

	// Any.toString: String
	// This extension has to be added for all data types
	data_type_t types[] = {
		TYPE_INT, TYPE_DOUBLE, TYPE_STRING, TYPE_UNIT, TYPE_BOOLEAN,
	};
	for (size_t i = 0; i < sizeof(types) / sizeof(data_type_t); i++)
	{
		char *id = generate_unique_ext_id("toString", types[i]);
		if (!id)
		{
			print_err("Failed to generate unique_name in symbtable_insert_builtins().", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		entry = symtable_item_create(ext_table, id);

		entry->symbol_type = SYM_EXTENSION;
		entry->data.extension.base_param.identifier = "value";
		entry->data.extension.base_param.type = types[i];
		entry->data.extension.is_defined = true;
		entry->data.extension.return_type = TYPE_STRING;

		free(id);
	}

	// Double.toInt: Int
	char *id = generate_unique_ext_id("toInt", TYPE_DOUBLE);
	if (!id)
	{
		print_err("Failed to generate unique_name in symbtable_insert_builtins().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	entry = symtable_item_create(ext_table, id);

	entry->symbol_type = SYM_EXTENSION;
	entry->data.extension.base_param.identifier = "value";
	entry->data.extension.base_param.type = TYPE_DOUBLE;
	entry->data.extension.is_defined = true;
	entry->data.extension.return_type = TYPE_INT;

	free(id);

	// Double.floor: Double
	id = generate_unique_ext_id("floor", TYPE_DOUBLE);
	if (!id)
	{
		print_err("Failed to generate unique_name in symbtable_insert_builtins().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	entry = symtable_item_create(ext_table, id);

	entry->symbol_type = SYM_EXTENSION;
	entry->data.extension.base_param.identifier = "value";
	entry->data.extension.base_param.type = TYPE_DOUBLE;
	entry->data.extension.is_defined = true;
	entry->data.extension.return_type = TYPE_DOUBLE;

	free(id);

	// String.length: Int
	id = generate_unique_ext_id("length", TYPE_STRING);
	if (!id)
	{
		print_err("Failed to generate unique_name in symbtable_insert_builtins().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	entry = symtable_item_create(ext_table, id);

	entry->symbol_type = SYM_EXTENSION;
	entry->data.extension.base_param.identifier = "value";
	entry->data.extension.base_param.type = TYPE_STRING;
	entry->data.extension.is_defined = true;
	entry->data.extension.return_type = TYPE_INT;

	return ERR_OK;
}
