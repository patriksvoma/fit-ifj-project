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
	if (key == NULL)
		return NULL;

	tree_node_t *new = malloc(sizeof(tree_node_t));

	if (new == NULL)
		return NULL;

	new->key = malloc(strlen(key) + 1);

	if (new->key == NULL)
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
