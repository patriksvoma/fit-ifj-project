/**
 * @file expression.h
 * @brief Operator-Precedence part of the syntax analyzer in the IFJ26 compiler, used for parsing expressions
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-14
 */

#pragma once

#include "error.h"
#include "parser.h"

/**
 * @brief Operators in the precedence table
 *
 */
typedef enum
{
	N, // None
	S, // Shift <
	E, // Equal =
	R  // Reduce >
} prec_table_operator_t;

/**
 * @brief Terminals in the precedence table
 *
 */
typedef enum
{
	TERMINAL_I,				//
	TERMINAL_LEFT_BRACKET,	// (
	TERMINAL_RIGHT_BRACKET, // )
	TERMINAL_DOT,			// .
	TERMINAL_U_NEG,			// ! (unary)
	TERMINAL_U_PLUS,		// + (unary)
	TERMINAL_U_MINUS,		// - (unary)
	TERMINAL_MUL,			// *
	TERMINAL_DIV,			// /
	TERMINAL_ADD,			// +
	TERMINAL_SUB,			// -
	TERMINAL_LT,			// <
	TERMINAL_LET,			// <=
	TERMINAL_GT,			// >
	TERMINAL_GET,			// >=
	TERMINAL_EQ,			// ==
	TERMINAL_NE,			// !=
	TERMINAL_AND,			// &&
	TERMINAL_OR,			// ||
	TERMINAL_DOLLAR			// $
} prec_table_terminal_t;

/**
 * @brief Parses an expression
 *
 * @param pdata Parser data
 * @param return_node Address to which the address of the created AST note should be written
 * @return err_t Error code
 */
err_t parse_expression(parser_data_t *pdata, ast_node_t **return_node);
