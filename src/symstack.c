/**
 * @file symstack.c
 * @brief Symbol stack for the expression parser
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-15
 */

#include "symstack.h"

err_t symstack_init(symstack_t *stack)
{
	// TODO: Implement
	(void)stack;
	print_err("symstack_init() is not implemented yet.", ERR_INTERNAL);
	return ERR_OK;
}

err_t symstack_destroy(symstack_t *stack)
{
	// TODO: Implement
	(void)stack;
	print_err("symstack_destroy() is not implemented yet.", ERR_INTERNAL);
	return ERR_OK;
}

err_t symstack_push(symstack_t *stack)
{
	// TODO: Implement
	(void)stack;
	print_err("symstack_push() is not implemented yet.", ERR_INTERNAL);
	return ERR_OK;
}

err_t symstack_pop(symstack_t *stack)
{
	// TODO: Implement
	(void)stack;
	print_err("symstack_pop() is not implemented yet.", ERR_INTERNAL);
	return ERR_OK;
}

symstack_item_t *symstack_top(symstack_t *stack)
{
	// TODO: Implement
	(void)stack;
	print_err("symstack_top() is not implemented yet.", ERR_INTERNAL);
	return ERR_OK;
}

symstack_item_t *symstack_top_terminal(symstack_t *stack)
{
	// TODO: Implement
	(void)stack;
	print_err("symstack_top_terminal() is not implemented yet.", ERR_INTERNAL);
	return ERR_OK;
}
