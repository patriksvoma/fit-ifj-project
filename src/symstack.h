/**
 * @file symstack.h
 * @brief Symbol stack for the expression parser
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-14
 */

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
 * @return err_t Error code
 */
err_t symstack_init();

/**
 * @brief Destroys all elements of the stack
 *
 * @return err_t Error code
 */
err_t symstack_destroy();

/**
 * @brief Pushes a symbol to the top of the stack
 *
 * @return err_t Error code
 */
err_t symstack_push();

/**
 * @brief Pops a symbol from the top of the stack
 *
 * @return err_t Error code
 */
err_t symstack_pop();

/**
 * @brief Returns a pointer to the top item of the symbol stack
 *
 * @return symstack_item_t Pointer to the top of the stack, NULL if the stack is empty
 */
symstack_item_t *symstack_top();
