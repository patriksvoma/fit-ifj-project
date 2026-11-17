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

err_t symtable_destroy(symbol_tree_t *tree)
{
    if (tree == NULL)
    {
        print_err("NULL pointer in symtable_destroy().", ERR_INTERNAL);
        return ERR_INTERNAL;
    }

    destroy_tree(tree->root);
    tree->root = NULL;

    return ERR_OK;
}
