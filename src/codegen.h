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
 * @brief Function for code generation
 *
 * @param root Pointer to node of ast containing the root
 * @param parser_data Pointer to struct with parser data
 * @return err_t Error code
 */
err_t generate_code(const ast_node_t *root);
