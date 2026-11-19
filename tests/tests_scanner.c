/**
 * @file tests_scanner.c
 * @brief Tests for scanner
 *
 * @author Jan Šerák (xserakj00@stud.fit.vut.cz)
 *
 * @date 2026-19-11
 */

#define _POSIX_C_SOURCE 200809L

#include "unity.h"

#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRINGS 64

extern unsigned scanner_line;
extern unsigned scanner_column;
extern unsigned scanner_prev_column;

static char input_path[64];
static char *strings[MAX_STRINGS];
static int string_count;

void setUp()
{
	input_path[0] = '\0';
	string_count = 0;
}

void tearDown()
{
	for (int i = 0; i < string_count; i++)
		free(strings[i]);

	string_count = 0;

	if (input_path[0] != '\0')
		unlink(input_path);
}

// Helpers
/**
 * @brief Write the source code to a temporary file and reopen stdin from it
 *
 * @param source Source code to scan
 */
static void set_input(const char *source)
{
	if (input_path[0] != '\0')
		unlink(input_path);

	strcpy(input_path, "/tmp/ifj26_scanner_XXXXXX");
	int fd = mkstemp(input_path);
	TEST_ASSERT_NOT_EQUAL(-1, fd);

	size_t len = strlen(source);
	TEST_ASSERT_EQUAL((ssize_t)len, write(fd, source, len));
	close(fd);

	TEST_ASSERT_NOT_NULL(freopen(input_path, "r", stdin));

	scanner_line = 1;
	scanner_column = 0;
	scanner_prev_column = 0;
}

/**
 * @brief Remember a string from a token so tearDown can free it
 *
 * @param token Token whose string should be freed later
 */
static void track(const token_t *token)
{
	if (token->attribute.string != NULL && string_count < MAX_STRINGS)
		strings[string_count++] = token->attribute.string;
}

/**
 * @brief Read the next token and expect it to succeed
 *
 * @return token_t The token
 */
static token_t next_ok(void)
{
	token_t token;
	TEST_ASSERT_EQUAL(ERR_OK, get_next_token(&token, EOL_OPTIONAL));
	track(&token);
	return token;
}

/**
 * @brief Read tokens until EOF or an error
 *
 * @return err_t ERR_OK if EOF was reached, otherwise the first error
 */
static err_t scan_all(void)
{
	for (int i = 0; i < 1000; i++)
	{
		token_t token;
		err_t err = get_next_token(&token, EOL_OPTIONAL);

		if (err != ERR_OK)
			return err;

		track(&token);

		if (token.type == TOKEN_EOF)
			return ERR_OK;
	}

	TEST_FAIL_MESSAGE("scanner did not reach EOF");
	return ERR_INTERNAL;
}

/**
 * @brief Expect that scanning the whole input ends with the given error
 *
 * @param source Source code to scan
 * @param expected Expected error code
 */
static void expect_error(const char *source, err_t expected)
{
	set_input(source);
	TEST_ASSERT_EQUAL_MESSAGE(expected, scan_all(), source);
}

/**
 * @brief Expect that the input consists of exactly the given token types followed by EOF
 *
 * @param source Source code to scan
 * @param types Expected token types
 * @param count Number of expected token types
 */
static void expect_types(const char *source, const token_type_t *types, size_t count)
{
	set_input(source);

	for (size_t i = 0; i < count; i++)
		TEST_ASSERT_EQUAL_MESSAGE(types[i], next_ok().type, source);

	TEST_ASSERT_EQUAL_MESSAGE(TOKEN_EOF, next_ok().type, source);
}

#define EXPECT_TYPES(source, ...)                                                                                      \
	expect_types(source, (token_type_t[]){__VA_ARGS__}, sizeof((token_type_t[]){__VA_ARGS__}) / sizeof(token_type_t))

// Basics
void test_empty_input_gives_eof()
{
	set_input("");
	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
}

void test_eof_is_returned_repeatedly()
{
	set_input("x");
	next_ok();
	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
}

void test_whitespace_is_skipped()
{
	set_input("   \r\n  ");
	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
}

void test_unknown_character_is_error()
{
	expect_error("#", ERR_SCANNER);
	expect_error("x ? y", ERR_SCANNER);
	expect_error("\\", ERR_SCANNER);
}

// Operators
void test_single_character_tokens()
{
	EXPECT_TYPES("( ) , : + - * ;", TOKEN_OPENING_BRACKET, TOKEN_CLOSING_BRACKET, TOKEN_COMMA, TOKEN_COLON, TOKEN_PLUS,
				 TOKEN_MINUS, TOKEN_ASTERISK, TOKEN_SEMICOLON);
}

void test_two_character_operators()
{
	EXPECT_TYPES("= == != ! < <= <- > >= && ||", TOKEN_ASSIGN, TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_NEG, TOKEN_LESS,
				 TOKEN_LESS_EQUAL, TOKEN_FOR_GEN_ASSIGN, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_AND, TOKEN_OR);
}

void test_operators_without_spaces()
{
	EXPECT_TYPES("a==b", TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER);
	EXPECT_TYPES("(a)", TOKEN_OPENING_BRACKET, TOKEN_IDENTIFIER, TOKEN_CLOSING_BRACKET);
	EXPECT_TYPES("===", TOKEN_EQUAL, TOKEN_ASSIGN);
}

void test_less_minus_is_generator_arrow()
{
	EXPECT_TYPES("x<-1", TOKEN_IDENTIFIER, TOKEN_FOR_GEN_ASSIGN, TOKEN_INT);
}

void test_single_ampersand_or_pipe_is_error()
{
	expect_error("a & b", ERR_SCANNER);
	expect_error("a | b", ERR_SCANNER);
}

void test_dot_token()
{
	EXPECT_TYPES("x.y", TOKEN_IDENTIFIER, TOKEN_DOT, TOKEN_IDENTIFIER);
	EXPECT_TYPES(".", TOKEN_DOT);
}

// Identifiers and keywords
void test_identifier()
{
	const char *names[] = {"x_1", "_abc", "__", "abc_"};

	set_input("x_1 _abc __ abc_");

	for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++)
	{
		token_t token = next_ok();
		TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
		TEST_ASSERT_EQUAL_STRING(names[i], token.attribute.string);
	}

	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
}

void test_keywords()
{
	keyword_t expected[] = {KW_DEF,	 KW_DO,	   KW_DOUBLE, KW_ELSE,	  KW_IF,		KW_IMPORT,
							KW_INT,	 KW_RETURN, KW_STRING, KW_THEN,	  KW_UNIT,		KW_VAL,
							KW_VAR,	 KW_WHILE,  KW_BOOLEAN, KW_EXTENSION, KW_FOR,		KW_MAIN};

	set_input("def do Double else if import Int return String then Unit val var while Boolean extension for @main");

	for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++)
	{
		token_t token = next_ok();
		TEST_ASSERT_EQUAL(TOKEN_KEYWORD, token.type);
		TEST_ASSERT_EQUAL(expected[i], token.attribute.keyword);
	}

	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
}

void test_keywords_are_case_sensitive()
{
	EXPECT_TYPES("int double If whilex", TOKEN_IDENTIFIER, TOKEN_IDENTIFIER, TOKEN_IDENTIFIER, TOKEN_IDENTIFIER);
}

void test_boolean_literals()
{
	set_input("true false truex");

	token_t token = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_BOOLEAN, token.type);
	TEST_ASSERT_TRUE(token.attribute.boolean);

	token = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_BOOLEAN, token.type);
	TEST_ASSERT_FALSE(token.attribute.boolean);

	TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, next_ok().type);
}

void test_lone_underscore()
{
	EXPECT_TYPES("_ ( _ )", TOKEN_UNDERSCORE, TOKEN_OPENING_BRACKET, TOKEN_UNDERSCORE, TOKEN_CLOSING_BRACKET);
}

void test_invalid_at_words()
{
	expect_error("@", ERR_SCANNER);
	expect_error("@mai", ERR_SCANNER);
	expect_error("@mainx", ERR_SCANNER);
	expect_error("@ main", ERR_SCANNER);
}

int main()
{
	UNITY_BEGIN();

	RUN_TEST(test_empty_input_gives_eof);
	RUN_TEST(test_eof_is_returned_repeatedly);
	RUN_TEST(test_whitespace_is_skipped);
	RUN_TEST(test_unknown_character_is_error);

	RUN_TEST(test_single_character_tokens);
	RUN_TEST(test_two_character_operators);
	RUN_TEST(test_operators_without_spaces);
	RUN_TEST(test_less_minus_is_generator_arrow);
	RUN_TEST(test_single_ampersand_or_pipe_is_error);
	RUN_TEST(test_dot_token);

	RUN_TEST(test_identifier);
	RUN_TEST(test_keywords);
	RUN_TEST(test_keywords_are_case_sensitive);
	RUN_TEST(test_boolean_literals);
	RUN_TEST(test_lone_underscore);
	RUN_TEST(test_invalid_at_words);

	return UNITY_END();
}
