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

typedef enum
{
	SYMBOL_VARIABLE,
	SYMBOL_FUNCTION,
} symbol_type_t;

typedef struct symbol_data
{
	symbol_type_t type;
	bool defined;
	int params;

	// TODO: additional data
} *symbol_data_t;

typedef struct symbol_node
{
	char *key;
	symbol_data_t data;
	struct symbol_node *left;
	struct symbol_node *right;
} symbol_node_t;

typedef struct symbol_tree
{
	symbol_node_t *root;
} symbol_tree_t;

typedef struct scope
{
	symbol_tree_t symbols;
	struct scope *parent;
} *scope_t;

// TODO:
//	symGetData?
//	scope stuff

/**
 * @brief Initializes symbol tree
 *
 * @param tree Pointer to the symbol tree which will be initialized
 * @return err_t Error code
 */
err_t symtable_init(symbol_tree_t *tree);

/**
 * @brief Inserts a symbol into the symbol tree
 *
 * @param tree Pointer to the symbol tree into which the symbol will be inserted
 * @param key Key of inserted symbol
 * @param data Data of inserted symbol
 * @return err_t Error code
 */
err_t symtable_insert(symbol_tree_t *tree, const char *key, symbol_data_t data);

/**
 * @brief Searches for a symbol in the symbol tree
 *
 * @param tree Pointer to the symbol tree which will be searched
 * @param key Key of the symbol
 * @return symbol_data_t Pointer to the symbol data if found, NULL if not found
 */
symbol_data_t symtable_search(symbol_tree_t tree, const char *key);

/**
 * @brief Removes a symbol from the symbol tree
 *
 * @param tree Pointer to the symbol tree
 * @param key Key of the symbol to be removed
 * @return err_t Error code
 */
err_t symtable_remove(symbol_tree_t *tree, const char *key);

/**
 * @brief Destroys all symbols of the symbol tree
 *
 * @param tree Pointer to the symbol tree which will be destroyed
 * @return err_t Error code
 */
err_t symtable_destroy(symbol_tree_t *tree);
