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
 * @brief Symbols in the precedence table
 *
 */
typedef enum
{
	N, // None
	S, // Shift <
	E, // Equal =
	R  // Reduce >
} prec_table_sign_t;

/**
 * @brief Indexes of the precedence table
 *
 */
typedef enum
{
	INDEX_I,			 //
	INDEX_LEFT_BRACKET,	 // (
	INDEX_RIGHT_BRACKET, // )
	INDEX_DOT,			 // .
	INDEX_U_NEG,		 // ! (unary)
	INDEX_U_PLUS,		 // + (unary)
	INDEX_U_MINUS,		 // - (unary)
	INDEX_MUL,			 // *
	INDEX_DIV,			 // /
	INDEX_ADD,			 // +
	INDEX_SUB,			 // -
	INDEX_LT,			 // <
	INDEX_LET,			 // <=
	INDEX_GT,			 // >
	INDEX_GET,			 // >=
	INDEX_EQ,			 // ==
	INDEX_NE,			 // !=
	INDEX_AND,			 // &&
	INDEX_OR,			 // ||
	INDEX_DOLLAR		 // $
} prec_table_index_t;

/**
 * @brief Parses an expression
 *
 * @param pdata Parser data
 * @param seed_token If not NULL, the seed_token is used as the first token in the expression
 * @return err_t Error code
 */
err_t parse_expression(parser_data_t *pdata, token_t *seed_token);
