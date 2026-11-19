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

typedef struct ast_node ast_node_t;

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
		long long int integer;
		double decimal;
		char *string;
		bool boolean;
	} value;
} literal_t;

// Struct for parameters
typedef struct param_node_list_t
{
	ast_node_t **param_node;
	size_t param_count;
} param_node_list_t;

// Types of for loops
typedef enum
{
	FOR_TO,
	FOR_UNTIL,
	FOR_TO_BY
} for_type_t;

typedef struct ast_node
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

		// Function definition
		struct
		{
			char *identifier;
			ast_node_t *block;
		} func_def;

		// Function calling
		struct
		{
			char *identifier;
			param_node_list_t **param_lists;
			size_t param_list_count;
		} func_call;

		// Extension definition
		struct
		{
			char *identifier;
			ast_node_t *block;
		} ext_def;

		// Extension calling
		struct
		{
			char *identifier;
			param_node_list_t **param_lists;
			size_t param_list_count;
			ast_node_t *base_node;
		} ext_call;

		// Code block
		struct
		{
			ast_node_t **statements;
			size_t statement_count;
		} block;

		// Unary operation
		struct
		{
			operator_type_t operator;
			ast_node_t *operand;
		} un_operator;

		// Binary operation
		struct
		{
			operator_type_t operator;
			ast_node_t *operand_left;
			ast_node_t *operand_right;
		} bin_operator;

		// Varible definition
		struct
		{
			char *identifier;
			ast_node_t *assigned_value;
		} variable_definition;

		// Variable assignment
		struct
		{
			char *identifier;
			ast_node_t *assigned_value;
		} variable_assignment;

		// Variable usage
		char *variable_use;

		// Return expression
		ast_node_t *return_call;

		// If statement
		struct
		{
			ast_node_t *condition;
			ast_node_t *if_block;
			ast_node_t *else_block;
		} if_statement;

		// While loop
		struct
		{
			ast_node_t *condition;
			ast_node_t *block;
		} while_loop;

		// For loop
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

		// Int to double conversion
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
 * @brief Adds function definition to program node
 *
 * @param program_node Node containing program
 * @param func_def_node Node containing the function definition
 * @return err_t Error code
 */
err_t ast_program_add_func_def(ast_node_t *program_node, ast_node_t *func_def_node);

/**
 * @brief Adds extension definition to program node
 *
 * @param program_node Node containing program
 * @param ext_def_node Node containing the extension definition
 * @return err_t Error code
 */
err_t ast_program_add_ext_def(ast_node_t *program_node, ast_node_t *ext_def_node);

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

/**
 * @brief Sets block for function definition
 *
 * @param func_def_node Pointer to node containing function definition
 * @param block Pointer to node containing block
 * @return err_t Error code
 */
err_t ast_func_def_set_block(ast_node_t *func_def_node, ast_node_t *block);

/**
 * @brief Makes node for function calling
 *
 * @param identificator Identificator of the function
 * @return ast_node_t* Pointer to created node
 */
ast_node_t *ast_func_call_make(const char *identificator);

/**
 * @brief Function that adds parameter to function calling
 *
 * @param func_call_node Node of function calling
 * @param param_node Node of parameter
 * @param param_list_index Index of parameter list
 * @return err_t Error code
 */
err_t ast_func_call_add_param(ast_node_t *func_call_node, ast_node_t *param_node, unsigned param_list_index);

/**
 * @brief Makes a node for extension definition
 *
 * @param extended_type Data type that can be extended
 * @return ast_node_t* Pointer to created node
 */
ast_node_t *ast_ext_def_make(data_type_t extended_type);

/**
 * @brief Sets the identificator in extension definition node
 *
 * @param func_def_node Node of extension definition
 * @param newId Pointer to an identificator. The string will be coppied into newly allocated memory
 * @return err_t Error code
 */
err_t ast_ext_def_set_id(ast_node_t *func_def_node, const char *newId);

/**
 * @brief Sets the block for extension
 *
 * @param func_def_node Node of extension definition
 * @param block Node of the block
 * @return err_t Error code
 */
err_t ast_ext_def_set_block(ast_node_t *func_def_node, ast_node_t *block);

/**
 * @brief Makes node for extension calling
 *
 * @param identificator Identificator of the extension
 * @return ast_node_t* Pointer to created node
 */
ast_node_t *ast_ext_call_make(const char *identificator);

/**
 * @brief Adds parameter to the extension
 *
 * @param ext_call_node Node of the extension calling
 * @param param_node Node of the parameter
 * @param param_list_index Index for the parameter list
 * @param param_index Index for the parameter inside of the list
 * @return err_t Error code
 */
err_t ast_ext_call_add_param(ast_node_t *ext_call_node, ast_node_t *param_node, unsigned param_list_index,
							 unsigned param_index);

/**
 * @brief Adds a base on wich the extension will be called
 *
 * @param ext_call_node Node of the extension calling
 * @param base_node Node for the base
 * @return err_t Error code
 */
err_t ast_ext_call_add_base(ast_node_t *ext_call_node, ast_node_t *base_node);

/**
 * @brief Makes a node for code block
 *
 * @return ast_node_t* Pointer to the block
 */
ast_node_t *ast_block_make();

/**
 * @brief Adds statement for a code block
 *
 * @param block_node Node for the code block
 * @param statement_node Node with the statement
 * @return err_t Error code
 */
err_t ast_block_add_statement(ast_node_t *block_node, ast_node_t *statement_node);

/**
 * @brief Makes an unary operation
 *
 * @param operator Type of operator
 * @param operand Operand of the operation
 * @return ast_node_t* Pointer to the node containing the operation
 */
ast_node_t *ast_unary_op_make(operator_type_t operator, ast_node_t * operand);

/**
 * @brief Makes an binary operation
 *
 * @param operator Type of operator
 * @param operand_left First operand of the operation
 * @param operand_right Second operand of the
 * @return ast_node_t* Pointer to the node containing the operation
 */
ast_node_t *ast_binary_op_make(operator_type_t operator, ast_node_t * operand_left, ast_node_t *operand_right);

/**
 * @brief Makes node for variable definition
 *
 * @param identifier Identifier of the variable
 * @return ast_node_t* Pointer to the node
 */
ast_node_t *ast_var_def_make(const char *identifier);

/**
 * @brief Sets a value for defined variable
 *
 * @param var_def_node Node with the definition
 * @param assigned_value_node Node with the value to be assigned
 * @return err_t Error code
 */
err_t ast_var_def_set_assignment(ast_node_t *var_def_node, ast_node_t *assigned_value_node);

/**
 * @brief Makes a node for variable assigment
 *
 * @param identifier The identifier for the variable
 * @param assigned_value Node with the value to be assigned
 * @return ast_node_t* Pointer to the node
 */
ast_node_t *ast_var_assign_make(const char *identifier, ast_node_t *assigned_value);

/**
 * @brief Makes a node for variable using
 *
 * @param identifier Identifier of the variable
 * @param var_type Data type of the variable
 * @return ast_node_t* Pointer to the node
 */
ast_node_t *ast_var_use_make(const char *identifier, data_type_t var_type);

/**
 * @brief Makes a node with expression to be returned
 *
 * @param return_value_node Node containing the value to be returned
 * @return ast_node_t* Pointer to the node
 */
ast_node_t *ast_return_call_make(ast_node_t *return_value_node);

/**
 * @brief Makes a node for if statement
 *
 * @param condition_node Node containing the condition
 * @return ast_node_t* Pointer to the node
 */
ast_node_t *ast_if_make(ast_node_t *condition_node);

/**
 * @brief Sets block for if branch
 *
 * @param if_node Node of the if statement
 * @param block Node containing the code block
 * @return err_t Error code
 */
err_t ast_if_set_then_block(ast_node_t *if_node, ast_node_t *block);

/**
 * @brief Sets block for else branch
 *
 * @param if_node Node of the if statement
 * @param block Node containing the code block
 * @return err_t Error code
 */
err_t ast_if_set_else_block(ast_node_t *if_node, ast_node_t *block);

/**
 * @brief Makes a node for while loop
 *
 * @param condition Condition of the while loop
 * @return ast_node_t* Pointer to the node
 */
ast_node_t *ast_while_make(ast_node_t *condition);

/**
 * @brief Sets the code block for the wile loop
 *
 * @param while_node Node containing the while loop
 * @param block Node containing the code block of the while loop
 * @return err_t Error code
 */
err_t ast_while_set_block(ast_node_t *while_node, ast_node_t *block);

/**
 * @brief Makes a node for for loop
 *
 * @param iterator_identifier Identifier of the iterator
 * @param from_node Node containing from expression
 * @return ast_node_t* Pointer to the node
 */
ast_node_t *ast_for_make(const char *iterator_identifier, ast_node_t *from_node);

/**
 * @brief Sets the type of for loop
 *
 * @param for_node Node containing the for loop
 * @param type Type of the for loop
 * @return err_t Error code
 */
err_t ast_for_set_type(ast_node_t *for_node, for_type_t type);

/**
 * @brief Sets the to expression in for loop
 *
 * @param for_node Node of the for loop
 * @param to_node Node of the to expression
 * @return err_t Error code
 */
err_t ast_for_set_to(ast_node_t *for_node, ast_node_t *to_node);

/**
 * @brief Sets the until expression in for loop
 *
 * @param for_node Node containing for loop
 * @param until_node Node containing until expression
 * @return err_t Error code
 */
err_t ast_for_set_until(ast_node_t *for_node, ast_node_t *until_node);

/**
 * @brief Sets the by expression in for loop
 *
 * @param for_node Node containing for loop
 * @param by_node Node containing by expression
 * @return err_t Error code
 */
err_t ast_for_set_by(ast_node_t *for_node, ast_node_t *by_node);

/**
 * @brief Sets the guard expression in for loop
 *
 * @param for_node Node containing for loop
 * @param guard_node Node containing guard expression
 * @return err_t Error code
 */
err_t ast_for_add_guard(ast_node_t *for_node, ast_node_t *guard_node);

/**
 * @brief Sets the code block in for loop
 *
 * @param for_node Node containing for loop
 * @param block Node containing the code block
 * @return err_t Error code
 */
err_t ast_for_set_block(ast_node_t *for_node, ast_node_t *block);

/**
 * @brief Makes node for int to double conversion
 *
 * @param operand Node with the number to be converted
 * @return ast_node_t* Pointer to the node
 */
ast_node_t *ast_int2double_make(ast_node_t *operand);

/**
 * @brief Destructs the ast
 *
 * @param root Root of the ast
 */
void destruct(ast_node_t *root);
