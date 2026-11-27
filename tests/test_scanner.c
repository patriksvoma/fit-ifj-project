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
	keyword_t expected[] = {KW_DEF, KW_DO,	   KW_DOUBLE,  KW_ELSE,		 KW_IF,	  KW_IMPORT,
							KW_INT, KW_RETURN, KW_STRING,  KW_THEN,		 KW_UNIT, KW_VAL,
							KW_VAR, KW_WHILE,  KW_BOOLEAN, KW_EXTENSION, KW_FOR,  KW_MAIN};

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

// Integer literals
void test_integer_literals()
{
	long long expected[] = {0, 7, 42, 1000000, 10};

	set_input("0 7 42 1_000_000 1__0");

	for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++)
	{
		token_t token = next_ok();
		TEST_ASSERT_EQUAL(TOKEN_INT, token.type);
		TEST_ASSERT_EQUAL_INT64(expected[i], token.attribute.integer);
	}

	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
}

void test_large_integer_fits_in_64_bits()
{
	set_input("9223372036854775807");

	token_t token = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_INT, token.type);
	TEST_ASSERT_EQUAL_INT64(9223372036854775807LL, token.attribute.integer);
}

void test_invalid_integers()
{
	expect_error("01", ERR_SCANNER);
	expect_error("0_1", ERR_SCANNER);
	expect_error("1_", ERR_SCANNER);
	expect_error("1_+2", ERR_SCANNER);
	expect_error("99999999999999999999", ERR_SCANNER);
}

void test_negative_number_is_minus_and_literal()
{
	EXPECT_TYPES("-5", TOKEN_MINUS, TOKEN_INT);
}

// Double literals
/**
 * @brief Expect the next token to be a double with exactly the given value
 *
 * Both values are parsed from the same decimal text, so they must be equal.
 *
 * @param expected Expected value
 */
static void expect_double(double expected)
{
	token_t token = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_DOUBLE, token.type);
	TEST_ASSERT_TRUE_MESSAGE(token.attribute.decimal == expected, "unexpected double value");
}

void test_double_literals()
{
	set_input("0.5 1.5 .5 1_0.2_5 3.14d 1.0D");

	expect_double(0.5);
	expect_double(1.5);
	expect_double(0.5);
	expect_double(10.25);
	expect_double(3.14);
	expect_double(1.0);

	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
}

void test_exponent_literals()
{
	set_input("1e10 1E+3 2.5e-3 0e5 .5e2 1e1_0 1e5d");

	expect_double(1e10);
	expect_double(1e3);
	expect_double(2.5e-3);
	expect_double(0.0);
	expect_double(50.0);
	expect_double(1e10);
	expect_double(1e5);

	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
}

void test_invalid_doubles()
{
	expect_error("1.5_", ERR_SCANNER);
	expect_error("1e", ERR_SCANNER);
	expect_error("1e+", ERR_SCANNER);
	expect_error("1e5_", ERR_SCANNER);
	expect_error("1e400", ERR_SCANNER);
}

// String literals
/**
 * @brief Expect the next token to be a string with the given value
 *
 * @param expected Expected value of the string
 */
static void expect_string(const char *expected)
{
	token_t token = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_STRING, token.type);
	TEST_ASSERT_NOT_NULL(token.attribute.string);
	TEST_ASSERT_EQUAL_STRING(expected, token.attribute.string);
}

void test_string_literal()
{
	set_input("\"ahoj svete\" x");
	expect_string("ahoj svete");
	TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, next_ok().type);
}

void test_empty_string_is_not_null()
{
	set_input("\"\" x");
	expect_string("");
	TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, next_ok().type);
}

void test_string_may_contain_tab()
{
	set_input("\"a\tb\"");
	expect_string("a\tb");
}

void test_escape_sequences()
{
	set_input("\"x\\ty\\nz\\r\\\"\\\\\"");
	expect_string("x\ty\nz\r\"\\");
}

void test_quote_inside_string()
{
	set_input("\"q\\\"q\"");
	expect_string("q\"q");
}

void test_invalid_strings()
{
	expect_error("\"abc", ERR_SCANNER);
	expect_error("\"ab\ncd\"", ERR_SCANNER);
	expect_error("\"a\\x\"", ERR_SCANNER);
	expect_error("\"a\\u0041\"", ERR_SCANNER);
	expect_error("\"a\\", ERR_SCANNER);
	expect_error("\"a\\\nb\"", ERR_SCANNER);
}

// Multiline string literals
void test_multiline_string_from_specification()
{
	set_input("\"\"\"A\n  hoj s\\nvete\n   \"\"\"");
	expect_string("A\n  hoj s\\nvete\n   ");
}

void test_multiline_string_with_quotes()
{
	set_input("\"\"\"a\"b\"\"c\"\"\"");
	expect_string("a\"b\"\"c");
}

void test_multiline_string_ends_with_last_three_quotes()
{
	set_input("\"\"\"x\"\"\"\"");
	expect_string("x\"");
	TEST_ASSERT_EQUAL(TOKEN_EOF, next_ok().type);
}

void test_empty_multiline_string()
{
	set_input("\"\"\"\"\"\" 1");
	expect_string("");
	TEST_ASSERT_EQUAL(TOKEN_INT, next_ok().type);
}

void test_unterminated_multiline_string()
{
	expect_error("\"\"\"abc", ERR_SCANNER);
	expect_error("\"\"\"abc\"", ERR_SCANNER);
	expect_error("\"\"\"abc\"\"", ERR_SCANNER);
}

// Position and lines
void test_token_line_numbers()
{
	set_input("a\nb\n\nc");

	TEST_ASSERT_EQUAL_UINT(1, next_ok().line);
	TEST_ASSERT_EQUAL_UINT(2, next_ok().line);
	TEST_ASSERT_EQUAL_UINT(4, next_ok().line);
}

void test_starts_logical_line_after_newline()
{
	set_input("a b\nc");

	TEST_ASSERT_FALSE(next_ok().starts_logical_line);
	TEST_ASSERT_FALSE(next_ok().starts_logical_line);
	TEST_ASSERT_TRUE(next_ok().starts_logical_line);
}

void test_token_ending_at_newline_keeps_newline()
{
	set_input("x = 5\nval");

	next_ok();
	next_ok();

	token_t number = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_INT, number.type);
	TEST_ASSERT_EQUAL_UINT(1, number.line);

	token_t keyword = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_KEYWORD, keyword.type);
	TEST_ASSERT_TRUE(keyword.starts_logical_line);
	TEST_ASSERT_EQUAL_UINT(2, keyword.line);
}

void test_eol_rules()
{
	set_input("a");
	token_t token;

	TEST_ASSERT_EQUAL(ERR_SYNTAX, get_next_token(&token, EOL_REQUIRED));

	set_input("a");
	TEST_ASSERT_EQUAL(ERR_OK, get_next_token(&token, EOL_OPTIONAL));
	TEST_ASSERT_EQUAL(ERR_OK, get_next_token(&token, EOL_FORBIDDEN));
}

void test_eol_rules_after_newline()
{
	set_input("a\nb");
	token_t token;

	TEST_ASSERT_EQUAL(ERR_OK, get_next_token(&token, EOL_OPTIONAL));
	TEST_ASSERT_EQUAL(ERR_SYNTAX, get_next_token(&token, EOL_FORBIDDEN));

	set_input("a\nb");
	TEST_ASSERT_EQUAL(ERR_OK, get_next_token(&token, EOL_OPTIONAL));
	TEST_ASSERT_EQUAL(ERR_OK, get_next_token(&token, EOL_REQUIRED));
}

void test_eol_rules_with_peeked_token()
{
	set_input("a\nb");
	token_t token;

	TEST_ASSERT_EQUAL(ERR_OK, get_next_token(&token, EOL_OPTIONAL));
	TEST_ASSERT_EQUAL(ERR_SYNTAX, peek_next_token(&token, EOL_FORBIDDEN));
	TEST_ASSERT_EQUAL(ERR_OK, get_next_token(&token, EOL_OPTIONAL));
}

void test_short_program()
{
	EXPECT_TYPES("val x: Int = 1_000 + 2.5e-3 * \"a\"", TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_COLON, TOKEN_KEYWORD,
				 TOKEN_ASSIGN, TOKEN_INT, TOKEN_PLUS, TOKEN_DOUBLE, TOKEN_ASTERISK, TOKEN_STRING);
}

// Comments
void test_division_is_not_comment()
{
	TEST_IGNORE_MESSAGE("comments not merged yet");

	EXPECT_TYPES("a / b", TOKEN_IDENTIFIER, TOKEN_DIV, TOKEN_IDENTIFIER);
	EXPECT_TYPES("a/b", TOKEN_IDENTIFIER, TOKEN_DIV, TOKEN_IDENTIFIER);
}

void test_line_comment_acts_as_newline()
{
	TEST_IGNORE_MESSAGE("comments not merged yet");

	set_input("a // komentar\nb");

	TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, next_ok().type);

	token_t token = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
	TEST_ASSERT_TRUE(token.starts_logical_line);
	TEST_ASSERT_EQUAL_UINT(2, token.line);
}

void test_line_comment_at_end_of_file()
{
	TEST_IGNORE_MESSAGE("comments not merged yet");

	EXPECT_TYPES("a // bez konce radku", TOKEN_IDENTIFIER);
	EXPECT_TYPES("//\n//\nx", TOKEN_IDENTIFIER);
}

void test_block_comment_acts_as_space()
{
	TEST_IGNORE_MESSAGE("comments not merged yet");

	set_input("a/* x */b");

	TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, next_ok().type);

	token_t token = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
	TEST_ASSERT_FALSE(token.starts_logical_line);
}

void test_nested_block_comments()
{
	TEST_IGNORE_MESSAGE("comments not merged yet");

	EXPECT_TYPES("/* a /* b */ c */ x", TOKEN_IDENTIFIER);
	EXPECT_TYPES("/* /* /* */ */ */ x", TOKEN_IDENTIFIER);
}

void test_block_comment_with_extra_characters()
{
	TEST_IGNORE_MESSAGE("comments not merged yet");

	EXPECT_TYPES("/** x **/ y", TOKEN_IDENTIFIER);
	EXPECT_TYPES("/***/x", TOKEN_IDENTIFIER);
	EXPECT_TYPES("/*//* x */ y */ z", TOKEN_IDENTIFIER);
}

void test_unterminated_block_comment()
{
	expect_error("/* abc", ERR_SCANNER);
	expect_error("/* abc *", ERR_SCANNER);
	expect_error("/* /* */", ERR_SCANNER);
}

void test_multiline_block_comment_acts_as_newline()
{
	TEST_IGNORE_MESSAGE("comments not merged yet");

	set_input("a /* x\ny */\nb");

	TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, next_ok().type);

	token_t token = next_ok();
	TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
	TEST_ASSERT_TRUE(token.starts_logical_line);
	TEST_ASSERT_EQUAL_UINT(3, token.line);
}

void test_token_after_multiline_block_comment_is_error()
{
	TEST_IGNORE_MESSAGE("comments not merged yet");

	expect_error("a /* x\ny */ b", ERR_SYNTAX);
	expect_error("/* \n */ \"s\"", ERR_SYNTAX);
}

void test_multiline_block_comment_before_eof_or_line_comment()
{
	TEST_IGNORE_MESSAGE("comments not merged yet");

	EXPECT_TYPES("a /* \n */", TOKEN_IDENTIFIER);
	EXPECT_TYPES("a /* \n */ // ok\nb", TOKEN_IDENTIFIER, TOKEN_IDENTIFIER);
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

	RUN_TEST(test_integer_literals);
	RUN_TEST(test_large_integer_fits_in_64_bits);
	RUN_TEST(test_invalid_integers);
	RUN_TEST(test_negative_number_is_minus_and_literal);

	RUN_TEST(test_double_literals);
	RUN_TEST(test_exponent_literals);
	RUN_TEST(test_invalid_doubles);

	RUN_TEST(test_string_literal);
	RUN_TEST(test_empty_string_is_not_null);
	RUN_TEST(test_string_may_contain_tab);
	RUN_TEST(test_escape_sequences);
	RUN_TEST(test_quote_inside_string);
	RUN_TEST(test_invalid_strings);

	RUN_TEST(test_multiline_string_from_specification);
	RUN_TEST(test_multiline_string_with_quotes);
	RUN_TEST(test_multiline_string_ends_with_last_three_quotes);
	RUN_TEST(test_empty_multiline_string);
	RUN_TEST(test_unterminated_multiline_string);

	RUN_TEST(test_token_line_numbers);
	RUN_TEST(test_starts_logical_line_after_newline);
	RUN_TEST(test_token_ending_at_newline_keeps_newline);
	RUN_TEST(test_eol_rules);
	RUN_TEST(test_eol_rules_after_newline);
	RUN_TEST(test_eol_rules_with_peeked_token);
	RUN_TEST(test_short_program);

	RUN_TEST(test_division_is_not_comment);
	RUN_TEST(test_line_comment_acts_as_newline);
	RUN_TEST(test_line_comment_at_end_of_file);
	RUN_TEST(test_block_comment_acts_as_space);
	RUN_TEST(test_nested_block_comments);
	RUN_TEST(test_block_comment_with_extra_characters);
	RUN_TEST(test_unterminated_block_comment);
	RUN_TEST(test_multiline_block_comment_acts_as_newline);
	RUN_TEST(test_token_after_multiline_block_comment_is_error);
	RUN_TEST(test_multiline_block_comment_before_eof_or_line_comment);

	return UNITY_END();
}
