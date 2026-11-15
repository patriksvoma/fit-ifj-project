/**
 * @file symstack.h
 * @brief Symbol stack for the expression parser
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-14
 */

#pragma once

#include "ast.h"
#include "error.h"
#include "expression.h"

/**
 * @brief Type of symbol stack item
 *
 */
typedef enum
{
	STACKITEM_TYPE_OPERATOR,
	STACKITEM_TYPE_TERMINAL,
	STACKITEM_TYPE_EXPRESSION
} symstack_item_type_t;

/**
 * @brief Symbol stack item
 *
 */
typedef struct symstack_item
{
	struct symstack_item *next;
	token_t source_token;
	symstack_item_type_t type;

	union {
		prec_table_sign_t operator;
		prec_table_index_t terminal;
		ast_node_t expression_node;
	} data;
} symstack_item_t;

/**
 * @brief Symbol stack
 *
 */
typedef struct
{
	symstack_item_t *top;
} symstack_t;

/**
 * @brief Initializes the symbol stack with a $ at the bottom
 *
 * @param stack Pointer to the stack which will be initialized
 * @return err_t Error code
 */
err_t symstack_init(symstack_t *stack);

/**
 * @brief Destroys all elements of the stack
 *
 * @param stack Pointer to the stack which will be destroyed
 * @return err_t Error code
 */
err_t symstack_destroy(symstack_t *stack);

/**
 * @brief Pushes a symbol to the top of the stack
 *
 * @param stack Pointer to the stack
 * @param item Item to push
 * @return err_t Error code
 */
err_t symstack_push(symstack_t *stack, symstack_item_t *item);

/**
 * @brief Pops a symbol from the top of the stack
 *
 * @param stack Pointer to the stack
 * @return err_t Error code
 */
err_t symstack_pop(symstack_t *stack);

/**
 * @brief Returns a pointer to the top item of the symbol stack
 *
 * @param stack Pointer to the stack
 * @return symstack_item_t Pointer to the top of the stack, NULL if the stack is empty
 */
symstack_item_t *symstack_top(const symstack_t *stack);

/**
 * @brief Returns a pointer to a terminal item closest to the stack top
 *
 * @param stack Pointer to the stack
 * @return symstack_item_t* Pointer to the terminal stack item, NULL if the stack is empty
 */
symstack_item_t *symstack_top_terminal(const symstack_t *stack);
