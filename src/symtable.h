/**
 * @file symtable.h
 * @brief Symbol table for the IFJ26 compiler
 *
 * @author Jakub Kvasnička (xkvasnj00@stud.fit.vut.cz)
 *
 * @date 2026-11-13
 */

#pragma once

#include "ast.h"
#include "error.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Type of symbol
 *
 */
typedef enum
{
	SYM_VARIABLE, // TODO: save Type (int, ...), index of variable in struct
	SYM_FUNCTION, // TODO: save Type (return), parameters (I/O), if already defined (collision check)
	SYM_EXTENSION,
} symbol_type_t;

typedef struct
{
	char *identifier;
	data_type_t type;
} param_t;

typedef struct
{
	param_t **params;
	size_t param_count;
} param_list_t;

/**
 * @brief Additional data of the symbol differs if the symbol is function, variable or extension
 *
 */
typedef struct symbol_data
{
	symbol_type_t symbol_type;

	union {
		struct
		{
			data_type_t return_type;
			param_list_t **param_lists;
			size_t param_list_count;
			char *unique_name;
			bool is_defined;
			bool is_main;
		} function;
		struct
		{
			data_type_t return_type;
			param_list_t **param_lists;
			size_t param_list_count;
			char *unique_name;
			bool is_defined;
			data_type_t extension_type;
		} extension;
		struct
		{
			data_type_t type;
			bool mutable;
			char *unique_name;
		} variable;
	} data;
} symbol_data_t;

/**
 * @brief Node of the binary search tree
 *
 */
typedef struct tree_node
{
	char *key;
	symbol_data_t data;

	int height;

	struct tree_node *left;
	struct tree_node *right;
} tree_node_t;

/**
 * @brief
 *
 */
typedef struct symtable
{
	struct symtable *parent;
	tree_node_t *root_node;

} symtable_t;

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
tree_node_t *symtable_create_node(const char *key, symbol_data_t data);

/**
 * @brief Initializes symbol tree
 *
 * @param tree Pointer to the symbol tree which will be initialized
 * @return err_t Error code
 */
err_t symtable_init(tree_node_t **root);

/**
 * @brief Inserts a symbol into the symbol tree
 *
 * @param tree Pointer to the symbol tree into which the symbol will be inserted
 * @param key Key of inserted symbol
 * @param data Data of inserted symbol
 * @return err_t Error code
 */
err_t symtable_insert(tree_node_t **root, const char *key, symbol_data_t data);

/**
 * @brief Searches for a symbol in the symbol tree
 *
 * @param tree Pointer to the symbol tree which will be searched
 * @param key Key of the symbol
 * @return symbol_data_t Pointer to the symbol data if found, NULL if not found
 */
symbol_data_t *symtable_search(tree_node_t *root, const char *key);

/**
 * @brief Destroys all symbols of the symbol tree
 *
 * @param tree Pointer to the symbol tree which will be destroyed
 * @return err_t Error code
 */
err_t symtable_destroy(tree_node_t **root);
