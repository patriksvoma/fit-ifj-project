/**
 * @file semantics.h
 * @brief Additional semantic checks
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-21
 */

#pragma once

#include "ast.h"
#include "parser.h"

/**
 * @brief Goes through the AST recursively and performs additional semantic checks
 *
 * @param program_node AST program node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t do_semantic_checks(ast_node_t *program_node, compiler_data_t *cdata);
