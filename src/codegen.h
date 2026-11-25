/**
 * @file codegen.h
 * @brief Header file for code generation
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-19
 */
#pragma once

#include "ast.h"
#include "parser.h"

/**
 * @brief Structure to help make labels for if, for, while and build-in read functions
 *
 */
typedef struct names_counter
{
	char *func_name;
	unsigned if_counter;
	unsigned for_counter;
	unsigned while_counter;
	unsigned read_val_counter;
} names_counter;

/**
 * @brief Function for code generation
 *
 * @param root Pointer to node of ast containing the root
 * @param cdata Pointer to the struct with compiler data
 * @return err_t Error code
 */
err_t generate_code(const ast_node_t *root, const compiler_data_t *cdata);

/**
 * @brief Function to generate code for a specific node
 *
 * @param node Node for wich the code should be generated
 */
err_t generate_nodes(const ast_node_t *node, symtable_t *symtables, names_counter *new_func_def);
