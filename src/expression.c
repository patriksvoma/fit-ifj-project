/**
 * @file expression.h
 * @brief Operator-Precedence part of the syntax analyzer in the IFJ26 compiler, used for parsing expressions
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-14
 */

#include "expression.h"

/**
 * @brief Symbols in the precedence table
 *
 */
typedef enum
{
	N, // None
	S, // Shift
	E, // Equal
	R  // Reduce
} prec_table_sign_t;

/**
 * @brief Indexes of the precedence table
 *
 */
typedef enum
{
	I_I,			 //
	I_LEFT_BRACKET,	 // (
	I_RIGHT_BRACKET, // )
	I_DOT,			 // .
	I_U_NEG,		 // - (unary)
	I_U_PLUS,		 // + (unary)
	I_U_MINUS,		 // ! (unary)
	I_MUL,			 // *
	I_DIV,			 // /
	I_ADD,			 // +
	I_SUB,			 // -
	I_LT,			 // <
	I_LET,			 // <=
	I_GT,			 // >
	I_GET,			 // >=
	I_EQ,			 // ==
	I_NE,			 // !=
	I_AND,			 // &&
	I_NOT,			 // ||
	I_DOLLAR		 // $
} prec_table_index_t;
