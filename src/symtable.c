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

symbol_data_t *symtable_search(tree_node_t *root, const char *key)
{
	if (key == NULL || root == NULL)
		return NULL;

	if (strcmp(key, root->key) == 0)
		return &root->data;
	else if (strcmp(key, root->key) < 0)
		symtable_search(root->left, key);

	return symtable_search(root->right, key);
}

static int height(const tree_node_t *node)
{
	if (node == NULL)
		return 0;

	return node->height;
}

static int max(int a, int b)
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
static int get_balance(const tree_node_t *node)
{
	if (node == NULL)
		return 0;

	return height(node->left) - height(node->right);
}

static tree_node_t *rotate_right(tree_node_t *y)
{
	tree_node_t *x = y->left;
	y->left = x->right;
	x->right = y;

	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

	return x;
}

static tree_node_t *rotate_left(tree_node_t *y)
{
	tree_node_t *x = y->right;
	y->right = x->left;
	x->left = y;

	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

	return x;
}

static tree_node_t *insert_node(tree_node_t *root, const char *key, symbol_data_t data)
{
	if (root == NULL)
		return symtable_create_node(key, data);

	if (strcmp(key, root->key) < 0)
		root->left = insert_node(root->left, key, data);
	else if (strcmp(key, root->key) > 0)
		root->right = insert_node(root->right, key, data);
	else
		return root;

	root->height = max(height(root->left), height(root->right)) + 1;
	int balance = get_balance(root);

	if (balance > 1 && strcmp(key, root->key) < 0) // LL
	{
		return rotate_right(root);
	}
	else if (balance < -1 && strcmp(key, root->key) > 0) // RR
	{
		return rotate_left(root);
	}
	else if (balance > 1 && strcmp(key, root->key) > 0) // LR
	{
		root->left = rotate_left(root->left);
		return rotate_right(root);
	}
	else if (balance < -1 && strcmp(key, root->key) < 0) // RL
	{
		root->right = rotate_right(root->right);
		return rotate_left(root);
	}

	return root;
}

err_t symtable_insert(tree_node_t **root, const char *key, symbol_data_t data)
{
	if (root == NULL || key == NULL)
		return ERR_INTERNAL;

	*root = insert_node(*root, key, data);

	return ERR_OK;
}
