/**
 * @file symtable.c
 * @brief Symbol table for the IFJ26 compiler
 *
 * @author Jakub Kvasnička (xkvasnj00@stud.fit.vut.cz)
 *
 * @date 2026-11-13
 */

#include "symtable.h"
#include <stdlib.h>
#include <string.h>

// TODO:
//	scope stuff
//	item_create
//	item_exists
//	item_find
//	destroy

/**
 * @brief Creates a new symbol tree node
 *
 * @param *key
 * @param data
 * @return tree_node_t Pointer to created node, NULL if allocation fails
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

/**
 * @brief Initializes symbol tree
 *
 * @param tree Pointer to the symbol tree which will be initialized
 * @return err_t Error code
 */
err_t symtable_init(tree_node_t **root)
{
	if (root == NULL)
	{
		print_err("NULL pointer in symtable_init().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	*root = NULL;

	return ERR_OK;
}

symbol_data_t *symtable_item_find(symtable_t *table, const char *key)
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
		return symtable_item_find(&left_table, key);
	}
	else
	{
		symtable_t right_table = *table;
		right_table.root_node = table->root_node->right;
		return symtable_item_find(&right_table, key);
	}
}

bool symtable_item_exists(symtable_t *table, const char *key)
{
	if (table == NULL || key == NULL)
		return false;

	if (symtable_item_find(table, key))
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
 * @return int Balance factor of the node
 */
int get_balance(const tree_node_t *node)
{
	if (node == NULL)
		return 0;

	return height(node->left) - height(node->right);
}

tree_node_t *rotate_right(tree_node_t *y)
{
	tree_node_t *x = y->left;
	y->left = x->right;
	x->right = y;

	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

	return x;
}

tree_node_t *rotate_left(tree_node_t *y)
{
	tree_node_t *x = y->right;
	y->right = x->left;
	x->left = y;

	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

	return x;
}

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

	return symtable_item_find(table, key);
}

bool symtable_item_defined(symtable_t *table, const char *key)
{
	if (table == NULL || key == NULL)
		return NULL;

	symbol_data_t *data = symtable_item_find(table, key);
	if (!data)
		return false;

	if (data->symbol_type == SYM_VARIABLE)
	{
		return true;
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
