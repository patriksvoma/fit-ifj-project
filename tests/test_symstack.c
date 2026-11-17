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
	TEST_ASSERT(true);
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_init);

	return UNITY_END();
}
