/**
 * @file symtable.h
 * @brief Symbol table for the IFJ26 compiler
 *
 * @author Jakub Kvasnička (xkvasnj00@stud.fit.vut.cz)
 *
 * @date 2026-11-13
 */

#pragma once

#include "error.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Type of symbol
 *
 */
typedef enum
{
	SYMBOL_VARIABLE,
	SYMBOL_FUNCTION,
} symbol_type_t;

/**
 * @brief Additional data of the symbol
 *
 */
typedef struct symbol_data
{
	symbol_type_t type;
	bool defined;
	size_t params;

	// TODO: additional data
} symbol_data_t;

/**
 * @brief Node of the binary search tree
 *
 */
typedef struct tree_node
{
	char *key;
	symbol_data_t data;
	struct tree_node *left;
	struct tree_node *right;
} tree_node_t;

/**
 * @brief Scope containing its own symbol table
 *
 */
typedef struct scope
{
	tree_node_t *symbols;
	struct scope *parent;
} scope_t;

// TODO:
//	symGetData?
//	scope stuff


/**
 * @brief Creates a new symbol tree node
 *
 * @return tree_node_t Pointer to created node, NULL if allocation fails
 */
tree_node_t *symtable_create_node();

/**
 * @brief Initializes symbol tree
 *
 * @param tree Pointer to the symbol tree which will be initialized
 * @return err_t Error code
 */
err_t symtable_init(tree_node_t **tree);

/**
 * @brief Inserts a symbol into the symbol tree
 *
 * @param tree Pointer to the symbol tree into which the symbol will be inserted
 * @param key Key of inserted symbol
 * @param data Data of inserted symbol
 * @return err_t Error code
 */
err_t symtable_insert(tree_node_t **tree, const char *key, symbol_data_t data);

/**
 * @brief Searches for a symbol in the symbol tree
 *
 * @param tree Pointer to the symbol tree which will be searched
 * @param key Key of the symbol
 * @return symbol_data_t Pointer to the symbol data if found, NULL if not found
 */
symbol_data_t *symtable_search(tree_node_t *tree, const char *key);
/**
 * @brief Removes a symbol from the symbol tree
 *
 * @param tree Pointer to the symbol tree
 * @param key Key of the symbol to be removed
 * @return err_t Error code
 */
err_t symtable_remove(tree_node_t **tree, const char *key);

/**
 * @brief Destroys all symbols of the symbol tree
 *
 * @param tree Pointer to the symbol tree which will be destroyed
 * @return err_t Error code
 */
err_t symtable_destroy(tree_node_t **tree);
