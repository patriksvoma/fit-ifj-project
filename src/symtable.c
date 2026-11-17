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

tree_node_t *symtable_create_node()
{
	tree_node_t *node = malloc(sizeof(tree_node_t));
	if (node == NULL)
		return NULL;

	node->key = NULL;
	node->data.type = NULL;
	return node;
}

err_t symtable_init(tree_node_t **tree)
{
	if (tree == NULL)
	{
		print_err("NULL pointer in symtable_inti().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
	*tree = NULL;

	return ERR_OK;
}

symbol_data_t *symtable_search(tree_node_t *tree, const char *key)
{
	if (tree == NULL)
	{
		return tree->data.defined = false;
	}

}

static void destroy_node(tree_node_t *node)
{
	if (node == NULL)
	{
		return;
	}

	destroy_node(node->left);
	destroy_node(node->right);

	free(node->key);
	free(node);
}

err_t symtable_destroy(tree_node_t **tree)
{
	if (tree == NULL)
	{
		print_err("NULL pointer in symtable_destroy().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	destroy_node(tree);
	tree = NULL;

	return ERR_OK;
}
