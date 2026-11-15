/**
 * @file symstack.c
 * @brief Symbol stack for the expression parser
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-15
 */

#include "symstack.h"

symstack_item_t *symstack_create_item()
{
	symstack_item_t *item = malloc(sizeof(symstack_item_t));
	if (!item)
		return NULL;

	item->next = NULL;
	item->source_token.type = TOKEN_NONE;

	return item;
}

err_t symstack_init(symstack_t *stack)
{
	if (!stack)
	{
		print_err("Null-reference in symstack_init().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	symstack_item_t *item = symstack_create_item();
	if (item == NULL)
	{
		print_err("Failed to allocate memory in symstack_init().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	item->type = STACKITEM_TYPE_TERMINAL;
	item->data.terminal = TERMINAL_DOLLAR;

	stack->top = item;

	return ERR_OK;
}

err_t symstack_destroy(symstack_t *stack)
{
	// TODO: Implement
	(void)stack;
	print_err("symstack_destroy() is not implemented yet.", ERR_INTERNAL);
	return ERR_OK;
}

err_t symstack_push(symstack_t *stack, symstack_item_t *item)
{
	if (!stack || !item)
	{
		print_err("Null-reference in symstack_push().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	item->next = stack->top;
	stack->top = item;

	return ERR_OK;
}

err_t symstack_pop(symstack_t *stack)
{
	if (!stack)
	{
		print_err("Null-reference in symstack_pop().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (stack->top == NULL)
	{
		print_err("Tried to pop an empty symstack.", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	symstack_item_t *item = stack->top;
	stack->top = item->next;

	free(item);

	return ERR_OK;
}

symstack_item_t *symstack_top(const symstack_t *stack)
{
	if (stack == NULL)
		return NULL;

	return stack->top;
}

symstack_item_t *symstack_top_terminal(const symstack_t *stack)
{
	if (stack == NULL || stack->top == NULL)
		return NULL;

	symstack_item_t *item = stack->top;

	while (item != NULL)
	{
		if (item->type == STACKITEM_TYPE_TERMINAL)
			return item;

		item = item->next;
	}

	return NULL;
}
