/**
 * @file ast.h
 * @brief Header file for Abstract syntax tree
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-11
 */

#pragma once

/**
 * @brief Data types
 *
 */
typedef enum
{
	TYPE_INT,
	TYPE_DOUBLE,
	TYPE_STRING,
	TYPE_UNIT,
	TYPE_BOOLEAN
} data_type_t;

/**
 * @brief Operatotions
 *
 */
typedef enum
{
	OP_UPLUS,
	OP_UMINUS,
	OP_ASTERISK,
	OP_DIV,
	OP_PLUS,
	OP_MINUS,
	OP_LESS,
	OP_LESS_EQUAL,
	OP_GREATER,
	OP_GREATER_EQUAL,
	OP_EQUAL,
	OP_NOT_EQUAL,
	OP_CONCATENATION,
	OP_NOT,
	OP_LOGIC_AND,
	OP_LOGIC_OR
} operator_type_t;

/**
 * @brief Node types
 *
 */
typedef enum
{
	AST_PROGRAM,
	AST_FUN_DEF,
	AST_PARAM,
	AST_RETURN,
	AST_BLOCK,
	AST_IF,
	AST_WHILE,
	AST_VAR_DEF,
	AST_IDENTIFIER,
	AST_LITERAL,
	AST_OPERATOR,
	AST_ASSIGN,
	AST_CALL,
} node_type_t;

