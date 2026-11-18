#include "unity.h"

#include "symstack.h"

static symstack_t stack;

void setUp()
{
	stack.top = NULL;
}

void tearDown()
{
	symstack_destroy(&stack);
}

void test_init()
{
	err_t result = symstack_init(&stack);
	TEST_ASSERT_EQUAL(result, ERR_OK);
	TEST_ASSERT_NOT_EQUAL(stack.top, NULL);

	TEST_ASSERT_EQUAL(stack.top->next, NULL);
	TEST_ASSERT_EQUAL(stack.top->type, STACKITEM_TYPE_TERMINAL);
	TEST_ASSERT_EQUAL(stack.top->data.terminal, TERMINAL_DOLLAR);
}

void test_destroy()
{
	TEST_ASSERT_EQUAL(ERR_OK, symstack_init(&stack));

	for (int i = 0; i < 2; i++)
	{
		symstack_item_t *item = symstack_create_item();
		TEST_ASSERT_NOT_NULL(item);
		TEST_ASSERT_EQUAL(ERR_OK, symstack_push(&stack, item));
	}

	TEST_ASSERT_NOT_NULL(stack.top);
	TEST_ASSERT_NOT_NULL(stack.top->next);
	TEST_ASSERT_NOT_NULL(stack.top->next->next);

	symstack_destroy(&stack);

	TEST_ASSERT_NULL(stack.top);
}

void test_push()
{
	// TODO:
}

void test_insert_before()
{
	// TODO:
}

void test_pop()
{
	// TODO:
}

void test_pop_multiple()
{
	// TODO:
}

void test_top()
{
	// TODO:
}

void test_top_terminal()
{
	// TODO:
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_init);
	RUN_TEST(test_destroy);

	return UNITY_END();
}
