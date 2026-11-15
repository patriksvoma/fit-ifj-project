/**
 * @file symstack.h
 * @brief Symbol stack for the expression parser
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-14
 */

#pragma once

#include "error.h"

typedef struct symstack_item_t
{
	struct symstack_item_t *next;
} symstack_item_t;

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
 * @return err_t Error code
 */
err_t symstack_push(symstack_t *stack);

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
symstack_item_t *symstack_top(symstack_t *stack);

/**
 * @brief Returns a pointer to a terminal item closest to the stack top
 *
 * @param stack Pointer to the stack
 * @return symstack_item_t* Pointer to the terminal stack item, NULL if the stack is empty
 */
symstack_item_t *symstack_top_terminal(symstack_t *stack);
