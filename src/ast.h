/**
 * @file ast.h
 * @brief Header file for Abstract syntax tree
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-11
 */

#pragma once

#include "error.h"
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
 * @brief Operations
 *
 */
typedef enum
{
	OP_UPLUS,
	OP_UMINUS,
	OP_MUL,
	OP_DIV,
	OP_PLUS,
	OP_MINUS,
	OP_LESS,
	OP_LESS_EQUAL,
	OP_GREATER,
	OP_GREATER_EQUAL,
	OP_EQUAL,
	OP_NOT_EQUAL,
	OP_NEG,
	OP_LOGIC_AND,
	OP_LOGIC_OR
} operator_type_t;

/**
 * @brief Node types
 *
 */
typedef enum
{
	AST_LITERAL_INT,
	AST_LITERAL_DOUBLE,
	AST_LITERAL_STRING,
	AST_LITERAL_BOOLEAN,
	AST_LITERAL_UNIT,
	AST_PROGRAM,
	AST_FUNC_DEF,
	AST_FUNC_CALL,
	AST_EXT_DEF,
	AST_EXT_CALL,
	AST_BLOCK,
	AST_UN_OPERATOR,
	AST_BIN_OPERATOR,
	AST_VAR_DEF,
	AST_VAR_ASSIGN,
	AST_VAR_USE,
	AST_RETURN_CALL,
	AST_IF,
	AST_WHILE,
	AST_FOR,
	AST_INT_TO_DOUBLE
} node_type_t;

/**
 * @brief Struct for literals
 *
 */
typedef struct
{
	data_type_t literal_type;
	union {
		int integer;
		double decimal;
		char *string;
		bool boolean;
	} value;
} literal_t;

typedef struct param_node_list_t
{
	ast_node_t **param_node;
	size_t param_count;
} param_node_list_t;

typedef enum
{
	FOR_TO,
	FOR_UNTIL,
	FOR_TO_BY
} for_type_t;

typedef struct ast_node_t
{
	node_type_t node_type;
	data_type_t eval_type;

	union {
		// Literals
		long long literal_int;
		double literal_double;
		char *literal_string;
		bool literal_boolean;

		// Main program block
		struct
		{
			ast_node_t **func_defs;
			size_t func_def_count;

			ast_node_t **ext_defs;
			size_t ext_def_count;
		} program;

		struct
		{
			char *identifier;
			ast_node_t *block;
		} func_def;

		struct
		{
			char *identifier;
			param_node_list_t **param_lists;
			size_t param_list_count;
		} func_call;

		struct
		{
			char *identifier;
			ast_node_t *block;
		} ext_def;

		struct
		{
			char *identifier;
			param_node_list_t **param_lists;
			size_t param_list_count;
			ast_node_t *base_node;
		} ext_call;

		struct
		{
			ast_node_t **statements;
			size_t statement_count;
		} block;

		struct
		{
			operator_type_t operator;
			ast_node_t *operand;
		} un_operator;

		struct
		{
			operator_type_t operator;
			ast_node_t *operand_left;
			ast_node_t *operand_right;
		} bin_operator;

		struct
		{
			char *identifier;
			ast_node_t *assigned_value;
		} variable_definition;

		struct
		{
			char *identifier;
			ast_node_t *assigned_value;
		} variable_assignment;

		char *variable_use;

		ast_node_t *return_call;

		struct
		{
			ast_node_t *condition;
			ast_node_t *if_block;
			ast_node_t *else_block;
		} if_statement;

		struct
		{
			ast_node_t *condition;
			ast_node_t *block;
		} while_loop;

		struct
		{
			for_type_t for_type;

			char *iterator_identifier;
			ast_node_t *from;
			ast_node_t *to;
			ast_node_t *until;
			ast_node_t *by;

			ast_node_t **guards;
			size_t guard_count;

			ast_node_t *block;
		} for_loop;

		ast_node_t *int2double;
	};
} ast_node_t;

/**
 * @brief Function to make a leaf containing literal
 *
 * @param literal Structure containing the literal value and type
 * @return ast_node_t* Pointer to leaf
 */
ast_node_t *ast_literal_make(literal_t *literal);

/**
 * @brief Function to make an empty node for program
 *
 * @return ast_node_t* Pointer to node of the program
 */
ast_node_t *ast_program_make();

/**
 * @brief Function to make a empty node containing function definition
 *
 * @return ast_node_t* Pointer to node of the function definition
 */
ast_node_t *ast_func_def_make();

/**
 * @brief Sets the identificator in a function definition node
 *
 * @param func_def_node The function definition AST node
 * @param newId Pointer to an identificator. The string will be coppied into newly allocated memory
 * @return err_t Error code
 */
err_t ast_func_def_set_id(ast_node_t *func_def_node, const char *new_id);
err_t ast_func_def_set_block(ast_node_t *func_def_node, ast_node_t *block);

ast_node_t *ast_func_call_make(char *identificator);
err_t ast_func_call_add_param(ast_node_t *func_call_node, ast_node_t *param_node, unsigned param_list_index,
							  unsigned param_index);

ast_node_t *ast_ext_def_make(data_type_t extended_type);
err_t ast_ext_def_set_id(ast_node_t *func_def_node, const char *newId);
err_t ast_ext_def_set_block(ast_node_t *func_def_node, ast_node_t *block);

ast_node_t *ast_ext_call_make(char *identificator);
err_t ast_ext_call_add_param(ast_node_t *ext_call_node, ast_node_t *param_node, unsigned param_list_index,
							 unsigned param_index);
err_t ast_ext_call_add_base(ast_node_t *ext_call_node, ast_node_t *base_node);

ast_node_t *ast_block_make();
err_t ast_block_add_statement(ast_node_t *block_node, ast_node_t *statement_node);

ast_node_t *ast_unary_op_make(operator_type_t *operator, ast_node_t * operand);

ast_node_t *ast_binary_op_make(operator_type_t *operator, ast_node_t * operand_left, ast_node_t *operand_right);

ast_node_t *ast_var_def_make(char *identifier);
err_t ast_var_def_set_assignment(ast_node_t *var_def_node, ast_node_t *assigned_value_node);

ast_node_t *ast_var_assign_make(char *identifier, ast_node_t *assigned_value);

ast_node_t *ast_var_use_make(char *identifier);

ast_node_t *ast_return_call_make(ast_node_t *return_value_node);

ast_node_t *ast_if_make(ast_node_t *condition_node);
err_t ast_if_set_then_block(ast_node_t *if_node, ast_node_t *block);
err_t ast_if_set_else_block(ast_node_t *if_node, ast_node_t *block);

ast_node_t *ast_while_make(ast_node_t *condition);
err_t ast_while_set_block(ast_node_t *while_node, ast_node_t *block);

ast_node_t *ast_for_make(char *iterator_identifier, ast_node_t *from_node);
err_t ast_for_set_type(ast_node_t *for_node, for_type_t type);
err_t ast_for_set_to(ast_node_t *for_node, ast_node_t *to_node);
err_t ast_for_set_until(ast_node_t *for_node, ast_node_t *until_node);
err_t ast_for_set_by(ast_node_t *for_node, ast_node_t *by_node);
err_t ast_for_add_guard(ast_node_t *for_node, ast_node_t *guard_node);
err_t ast_for_set_block(ast_node_t *for_node, ast_node_t *block);

ast_node_t *ast_int2double_make(ast_node_t *operand);
