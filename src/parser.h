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

/**
 * @brief Struct for holding the parser data
 *
 */
typedef struct
{
	token_t token;
} parser_data_t;

/**
 * @brief Main parsing function
 *
 * @param root_node AST root node
 * @return err_t Error code
 */
err_t parse(ast_node_t *root_node);
