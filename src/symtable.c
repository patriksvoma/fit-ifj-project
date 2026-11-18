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

	tree_node_t *node = malloc(sizeof(tree_node_t));

	if (!node)
		return NULL;

	node->key = malloc(strlen(key) + 1);

	if (!node->key)
	{
		free(node);
		return NULL;
	}

	strcpy(node->key, key);

	node->data = data;
	node->height = 1;
	node->left = NULL;
	node->right = NULL;
	return node;
}

err_t symtable_init(tree_node_t **tree)
{
	if (tree == NULL)
	{
		print_err("NULL pointer in symtable_init().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	*tree = NULL;

	return ERR_OK;
}

symbol_data_t *symtable_search(tree_node_t *tree, const char *key)
{
	if (key == NULL || tree == NULL)
		return NULL;

	int cmp = strcmp(key, tree->key);

	if (cmp == 0)
		return &tree->data;
	else if (cmp < 0)
		symtable_search(tree->left, key);

	return symtable_search(tree->right, key);
}

/**
 * @brief Get height of a node
 *
 * @param node Pointer to the node
 * @return int Height of node
 */
static int height(tree_node_t *node)
{
	if (node == NULL)
		return 0;

	return node->height;
}

/**
 * @brief Returns bigger value
 *
 * @param a Int value
 * @param b Int value
 * @return int Bigger value between a and b
 */
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
 * @param node Pointer to the node
 * @return int Balance factor of the node
 */
static int get_balance(tree_node_t *node)
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

	// updating height
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

	// updating height
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
	else if (balance < -1 && strcmp(key, root->key) < 0) //RL
	{
		root->right = rotate_right(root->right);
		return rotate_left(root);
	}

	return root;
}

err_t symtable_insert(tree_node_t **tree, const char *key, symbol_data_t data)
{
	if (tree == NULL || key == NULL)
		return ERR_INTERNAL;

	*tree = insert_node(*tree, key, data);

	return ERR_OK;

}
