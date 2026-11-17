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

void symstack_destroy(symstack_t *stack)
{
	if (!stack)
		return;

	symstack_item_t *current = stack->top;
	while (current)
	{
		symstack_item_t *next = current->next;
		free(current);
		current = next;
	}

	stack->top = NULL;
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
err_t symstack_insert_before(symstack_t *stack, symstack_item_t *item, const symstack_item_t *before)
{
	if (!stack || !item || !before || item->next != NULL)
	{
		print_err("Invalid item passed to symstack_insert_before().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	for (const symstack_item_t *current = stack->top; current != NULL; current = current->next)
	{
		if (current == item)
		{
			print_err("Item is already in the stack in symstack_insert_before().", ERR_INTERNAL);
			return ERR_INTERNAL;
		}
	}

	symstack_item_t **link = &stack->top;
	while (*link != NULL && *link != before)
		link = &(*link)->next;

	if (*link == NULL)
	{
		print_err("Before item is not in the stack in symstack_insert_before().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	item->next = *link;
	*link = item;
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

err_t symstack_pop_multiple(symstack_t *stack, unsigned count)
{
	for (unsigned i = 0; i < count; i++)
	{
		err_t result = symstack_pop(stack);
		if (result)
			return result;
	}

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
