/**
 * @file ast.h
 * @brief Header file for Abstract syntax tree
 *
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-11
 */

#pragma once

#include "scanner.h"
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
	OP_NEG,
	OP_LOGIC_AND,
	OP_LOGIC_OR // TODO: BOOL2INT, desetninne deleni
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
		// Blocks and program
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
		ast_node_t *expression;

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
	} ast_node_value;
};

/**
 * @brief Function to add a leaf containing literal
 *
 * @param literal Structure containing the literal value and type
 * @return ast_node_t Pointer to leaf
 */
ast_node_t *ast_add_literal(literal_t *literal);

/**
 * @brief Function to add a leaf containing identifier
 *
 * @param i Poiter to identifier name
 * @param i_type Data type of identifier
 * @return ast_node_t Pointer to leaf
 */
ast_node_t *ast_add_identifier(char *i, data_type_t i_type);

/**
 * @brief Function to make tree containing unary operation
 *
 * @param operand Poiter to node containing operand
 * @param op Type of operator
 * @return ast_node_t Pointer to node of the operation
 */
ast_node_t *ast_add_unary_op(ast_node_t *operand, operator_type_t op);

/**
 * @brief Function to make a tree containing binary operation
 *
 * @param operand_left Poiter to node containing left operand
 * @param operand_right Poiter to node containing right operand
 * @param op Type of operator
 * @return ast_node_t Pointer to node of the operation
 */
ast_node_t *ast_add_binary_op(ast_node_t *operand_left, ast_node_t *operand_right, operator_type_t op);

/**
 * @brief Function to make tree containing variable assigment
 *
 * @param i Poiter to identifier name
 * @param i_type Data type of identifier
 * @param expression Pointer to node with expression that should be assigned to identifier
 * @return ast_node_t Pointer to node with assigment
 */
ast_node_t *ast_add_assign(char *i, data_type_t i_type, ast_node_t *expression);

/**
 * @brief Function to make tree containing variable definition
 *
 * @param i Poiter to identifier name
 * @param i_type Data type of identifier
 * @param expression Pointer to node with expression that should be assigned to identifier
 * @param non_modifiable Bool to decide if its val or var
 * @return ast_node_t Pointer to node with variable definition
 */
ast_node_t *ast_add_var_def(char *i, data_type_t i_type, ast_node_t *expression, bool non_modifiable);

/**
 * @brief Function to make tree containing return expression
 *
 * @param expression Pointer to node with expression that should be returned
 * @return ast_node_t Pointer to node with return expression
 */
ast_node_t *ast_add_return(ast_node_t *expression);

/**
 * @brief Function to add a node containing function parameter
 *
 * @param i Poiter to parameter identifier name
 * @param i_type Data type of identifier
 * @return ast_node_t Pointer to node of the parameter
 */
ast_node_t *ast_add_param(char *i, data_type_t i_type);

/**
 * @brief Function to make a tree containing while cycle
 *
 * @param condition Poiter to node containing condition
 * @param block Pointer to node containing block of the while cycle
 * @return ast_node_t Pointer to node of the while cycle
 */
ast_node_t *ast_add_while(ast_node_t *condition, ast_node_t *block);

/**
 * @brief Function to make a tree containing if statement
 *
 * @param condition A node with condition for the statement
 * @param blocks An array of code blocks for each branch, first is if, last else, others else if
 * @param block_count Number of code blocks for the statement
 * @return ast_node_t Pointer to node of the if statement
 */
ast_node_t *ast_add_if(ast_node_t *condition, ast_node_t **blocks, size_t block_count);
