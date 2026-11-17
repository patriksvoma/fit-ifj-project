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
	OP_LOGIC_OR
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
	AST_LITERAL_BOOLEAN,
	AST_BOOL_TO_INT
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
		} code_block;

		// Literals
		int int_literal;
		double double_literal;
		char *string_literal;
		bool boolean_literal;

		// Variable
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
			ast_node_t *if_block;
			ast_node_t *else_block;
		} if_statement;

		// While
		struct
		{
			ast_node_t *condition;
			ast_node_t *block;
		} while_loop;

		// Bool to int
		ast_node_t *operand;
	} ast_node_value;
};

/**
 * @brief Function to make a leaf containing literal
 *
 * @param literal Structure containing the literal value and type
 * @return ast_node_t* Pointer to leaf
 */
ast_node_t *ast_make_literal(literal_t *literal);

/**
 * @brief Function to make a leaf containing variable
 *
 * @param i Poiter to variable name
 * @param i_type Data type of variable
 * @return ast_node_t* Pointer to leaf
 */
ast_node_t *ast_make_variable(char *i, data_type_t i_type);

/**
 * @brief Function to make tree containing unary operation
 *
 * @param operand Poiter to node containing operand
 * @param op Type of operator
 * @return ast_node_t* Pointer to node of the operation
 */
ast_node_t *ast_make_unary_op(ast_node_t *operand, operator_type_t op);

/**
 * @brief Function to make a tree containing binary operation
 *
 * @param operand_left Poiter to node containing left operand
 * @param operand_right Poiter to node containing right operand
 * @param op Type of operator
 * @return ast_node_t* Pointer to node of the operation
 */
ast_node_t *ast_make_binary_op(ast_node_t *operand_left, ast_node_t *operand_right, operator_type_t op);

/**
 * @brief Function to make tree containing variable assignment
 *
 * @param i Poiter to identifier name
 * @param i_type Data type of identifier
 * @param expression Pointer to node with expression that should be assigned to identifier
 * @return ast_node_t* Pointer to node with assignment
 */
ast_node_t *ast_make_assign(char *i, data_type_t i_type, ast_node_t *expression);

/**
 * @brief Function to make tree containing variable definition
 *
 * @param i Poiter to identifier name
 * @param i_type Data type of identifier
 * @param expression Pointer to node with expression that should be assigned to identifier
 * @param non_modifiable Bool to decide if its val or var
 * @return ast_node_t* Pointer to node with variable definition
 */
ast_node_t *ast_make_var_def(char *i, data_type_t i_type, ast_node_t *expression, bool non_modifiable);

/**
 * @brief Function to make tree containing return expression
 *
 * @param expression Pointer to node with expression that should be returned
 * @return ast_node_t* Pointer to node with return expression
 */
ast_node_t *ast_make_return(ast_node_t *expression);

/**
 * @brief Function to make a node containing function parameter
 *
 * @param i Poiter to parameter identifier
 * @param i_type Data type of identifier
 * @return ast_node_t* Pointer to node of the parameter
 */
ast_node_t *ast_make_param(char *i, data_type_t i_type);

/**
 * @brief Function to make a tree containing while cycle
 *
 * @param condition Poiter to node containing condition
 * @param block Pointer to root of tree containing block of the while cycle
 * @return ast_node_t* Pointer to node of the while cycle
 */
ast_node_t *ast_make_while(ast_node_t *condition, ast_node_t *block);

/**
 * @brief Function to make a tree containing if statement
 *
 * @param condition A node with condition for the statement
 * @param if_block Code block for if branch
 * @param else_block Code block for else branch
 * @return ast_node_t* Pointer to node of the if statement
 */
ast_node_t *ast_make_if(ast_node_t *condition, ast_node_t *if_block, ast_node_t *else_block);

/**
 * @brief Function to make a tree containing function calling
 *
 * @param i Poiter to function identifier name
 * @param args An array of arguments of the function
 * @param args_count Number of arguments of the function
 * @return ast_node_t* Pointer to node of the function call
 */
ast_node_t *ast_make_call(char *i);

/**
 * @brief Function to make a empty node containing function definition
 *
 * @param is_main Bool to decide if its main function
 * @return ast_node_t* Pointer to node of the function definition
 */
ast_node_t *ast_make_func_def(bool is_main);

/**
 * @brief Function to make an empty node for code block
 *
 * @return ast_node_t Pointer to node of the code block
 */
ast_node_t *ast_make_block();

/**
 * @brief Function to make an empty node for program
 *
 * @return ast_node_t* Pointer to node of the program
 */
ast_node_t *ast_make_program();

/**
 * @brief Function to destruct the tree
 *
 * @param root Pointer to root node
 * @return void
 */
void destruct(ast_node_t *root);
