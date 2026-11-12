/**
 * @file ast.h
 * @brief Header file for Abstract syntax tree
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-11
 */

#pragma once

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Data types
 *
 */
typedef enum
{
	TYPE_NONE,
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
	OP_LOGIC_OR		// TODO: BOOL2INT, desetninne deleni
} operator_type_t;

/**
 * @brief Node types
 *
 */
typedef enum
{
	AST_PROGRAM,
	AST_FUNC_DEF,
	AST_PARAM,
	AST_RETURN,
	AST_BLOCK,
	AST_IF,
	AST_WHILE,
	AST_VAR_DEF,
	AST_IDENTIFIER,
	AST_BIN_OPERATOR,
	AST_UN_OPERATOR,
	AST_ASSIGN,
	AST_CALL,
	AST_LITERAL_INT,
	AST_LITERAL_DOUBLE,
	AST_LITERAL_STRING,
	AST_LITERAL_UNIT,
	AST_LITERAL_BOOLEAN
} node_type_t;

typedef struct ast_node ast_node_t;

/**
 * @brief Struct for ast nodes
 *
 */
struct ast_node
{
	node_type_t node_type;
	data_type_t eval_type;

	union {
		struct
		{
			ast_node_t **expression;
			size_t exp_count;
		} program;

		// Literals
		int int_literal;
		double double_literal;
		char *string_literal;
		bool boolean_literal;

		// Identifier
		char *identifier;

		// Unary operator
		struct
		{
			operator_type_t operator_type;
			ast_node_t *operand;
		} un_operator;

		// Binary operator
		struct
		{
			operator_type_t operator_type;
			ast_node_t *operand_right;
			ast_node_t *operand_left;
		} bin_operator;

		// Assign
		struct
		{
			char *identifier;
			ast_node_t *expression;
		} assign;

		// Variable definition
		struct
		{
			char *identifier;
			ast_node_t *expression;
			data_type_t data_type;
			bool non_modifiable;
		} var_def;

		// Call function
		struct
		{
			char *identifier;
			ast_node_t **arguments;
			size_t arg_count;
		} call;

		// Return
		ast_node_t *return_var;

		// Function definition
		struct
		{
			char *identifier;
			bool is_main;
			ast_node_t *block;
			ast_node_t **params;
			size_t param_count;
			data_type_t return_type;
		} func_def;

		// Function parameter
		struct
		{
			char *identifier;
			data_type_t data_type;
		} param;

		// If statement
		struct
		{
			ast_node_t *condition;
			ast_node_t **block;
			size_t block_count;
		} if_statement;

		// While
		struct
		{
			ast_node_t *condition;
			ast_node_t *block;
		} while_loop;
	};
};
