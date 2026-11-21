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
	SYM_VARIABLE,
	SYM_FUNCTION,
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
 * @brief Additional data of the symbol, differs if the symbol is function, variable or extension
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
			param_t base_param;
		} extension;
		struct
		{
			data_type_t type;
			bool mutable;
			bool is_defined;
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
 * @brief Symbol table scope
 *
 * Each scope has its own tree and a pointer to its parent scope, allowing nesting
 */
typedef struct symtable
{
	struct symtable *parent;
	tree_node_t *root_node;
	unsigned child_table_counter;
} symtable_t;

/**
 * @brief Creates an empty item to symbol table
 *
 * @param table Pointer to the symbol table into which the empty item created
 * @param key Identifier of inserted symbol
 * @return symbol_data_t Pointer to data of the new item
 */
symbol_data_t *symtable_item_create(symtable_t *table, const char *key);

/**
 * @brief Searches for a symbol in the symbol tree
 *
 * @param table Pointer to the symbol table which will be searched
 * @param key Identifier of the symbol
 * @return symbol_data_t Pointer to the symbol data if found, NULL if not found
 */
symbol_data_t *symtable_item_find(symtable_t *table, const char *key);

/**
 * @brief Searches for a symbol in the symbol table
 *
 * @param table Pointer to the symbol table which will be searched
 * @param key Identifier of the symbol
 * @return bool If found True, False if not found
 */
bool symtable_item_exists(symtable_t *table, const char *key);

/**
 * @brief Searches for a symbol in the symbol table and parents and checks if it is defined
 *
 * @param table Pointer to the symbol tree which will be searched
 * @param key Identifier of the symbol
 * @return bool For variables, functions or extensions: true if found and defined,
 * 				false if not found or found and not defined
 */
bool symtable_item_defined(symtable_t *table, const char *key);

/**
 * @brief Searches for a symbol in the local symbol table and checks if it is defined
 *
 * @param table Pointer to the symbol tree which will be searched
 * @param key Identifier of the symbol
 * @return bool For variables, functions or extensions: true if found and defined,
 * 				false if not found or found and not defined
 */
bool symtable_item_defined_local(symtable_t *table, const char *key);

/**
 * @brief Adds an empty param list to a function or an extension
 *
 * @param symbol Function or extension symbol
 * @return err_t Error code
 */
err_t symbol_add_param_list(symbol_data_t *symbol);

/**
 * @brief Finds the parameter with the highest index in a parameter list with the highest index
 *
 * @param symbol Function or extension symbol
 * @return param_t Pointer to the parameter
 */
param_t *symbol_top_param(symbol_data_t *symbol);

/**
 * @brief Appends a new empty parameter into the parameter list with the highest index
 *
 * @param symbol Function or extension symbol
 * @return err_t Error code
 */
err_t symbol_add_param(symbol_data_t *symbol);

/**
 * @brief Destroys all symbols of the symbol tree
 *
 * @param table Pointer to the symbol tree which will be destroyed
 */
void symtable_destroy(symtable_t **table);

/**
 * @brief Allocates a new symbol table and with the "parent" as its parent
 *
 * @param parent Symbol table to use as the parent
 * @return symtable_t* Pointer to a new child table
 */
symtable_t *local_table_create(symtable_t *parent);

/**
 * @brief Deallocates the local table and returns a pointer to its parent
 *
 * @param table Table to destroy
 * @return symtable_t* Pointer to the parent table, if it exists
 */
symtable_t *local_table_destroy(symtable_t *table);

/**
 * @brief Generates a unique name for a symbol and saves it inside
 *
 * @param identifier Original identifier of the symbol
 * @param symbol Symbol which is changed
 * @param table Table, in which the symbol is located (used for generating the name)
 * @return err_t Error code
 */
err_t symbol_generate_unique_id(const char *identifier, symbol_data_t *symbol, const symtable_t *table);

/**
 * @brief Adds all parameters (and the base) of a function/extension into a symbol table as unmutable variables
 *
 * @param symbol Function / extension symbol
 * @param table Symbol table where the variables will be added
 * @return err_t Error code
 */
err_t symbol_add_parameters_into_table(symbol_data_t *func_symbol, symtable_t *table);
