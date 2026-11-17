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

static void destroy_node(symbol_node_t *node)
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

err_t symtable_destroy(symbol_tree_t *tree)
{
    if (tree == NULL)
    {
        print_err("NULL pointer in symtable_destroy().", ERR_INTERNAL);
        return ERR_INTERNAL;
    }

    destroy_node(tree->root);
    tree->root = NULL;

    return ERR_OK;
}
