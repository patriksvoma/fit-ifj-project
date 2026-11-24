/**
 * @file parser.h
 * @brief Recursive (top-down) part of the syntax analyzer for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#pragma once

#include "ast.h"
#include "error.h"
#include "scanner.h"
#include "symtable.h"

/**
 * @brief Struct for holding the compiler data
 *
 */
typedef struct
{
	token_t token;
	bool in_parenthesis;

	bool in_main;
	bool in_extension;
	param_t last_extension_param;

	symtable_t function_table;
	symtable_t extension_table;

	symtable_t *local_var_table;
	bool main_defined;
} compiler_data_t;

/**
 * @brief Main parsing function
 *
 * @param program_node AST root node
 * @return err_t Error code
 */
err_t parse(ast_node_t *program_node, compiler_data_t *cdata);
