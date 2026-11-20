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
	ast_node_t *node;

	union {
		prec_table_operator_t operator;
		prec_table_terminal_t terminal;
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
 * @brief Creates an empty item and returns the pointer to it
 *
 * @return symstack_item_t* Pointer to the newly created item
 */
symstack_item_t *symstack_create_item();

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
 */
void symstack_destroy(symstack_t *stack);

/**
 * @brief Pushes a symbol to the top of the stack
 *
 * @param stack Pointer to the stack
 * @param item Item to push
 * @return err_t Error code
 */
err_t symstack_push(symstack_t *stack, symstack_item_t *item);

/**
 * @brief Inserts a symbol before (from the top of the stack) a different item. The before item has to be a pointer to
 * an item inside the list.
 *
 * @param stack Symbol stack
 * @param item Item to be inserted
 * @param before Item before which the item will be inserted
 * @return err_t Error code
 */
err_t symstack_insert_before(symstack_t *stack, symstack_item_t *item, const symstack_item_t *before);

/**
 * @brief Pops a symbol from the top of the stack
 *
 * @param stack Pointer to the stack
 * @return err_t Error code
 */
err_t symstack_pop(symstack_t *stack);

/**
 * @brief Pops multiple symbols from the top of the stack
 *
 * @param stack Pointer to the stack
 * @param count How many symbols should be popped
 * @return err_t Error code
 */
err_t symstack_pop_multiple(symstack_t *stack, unsigned count);

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
