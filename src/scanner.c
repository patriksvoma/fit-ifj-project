/**
 * @file scanner.c
 * @brief Scanner module for the IFJ26 compiler
 *
 * @author Jan Šerák (xserakj00@stud.fit.vut.cz)
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "scanner.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief States of the scanner
 *
 */
typedef enum
{
	S_START,

	S_IDENTIFIER,
	S_UNDERSCORE,

	S_ZERO,
	S_NUMBER_INT,
	S_NUM_INT_UNDERSCORE,
	S_DOT,
	S_NUMBER_DOUBLE,
	S_NUM_DOUBLE_UNDERSCORE,
	S_EXPONENT,
	S_EXP_SIGN,
	S_EXP_END,
	S_EXP_END_UNDERSCORE,

	S_STRING_START_1,
	S_STRING,
	S_ESCAPE,
	S_STRING_START_2,
	S_MULTILINE_STRING,
	S_MULTILINE_END_1,
	S_MULTILINE_END_2,

	S_SLASH,
	S_COMMENT,
	S_COMMENT_BLOCK,
	S_COMMENT_BLOCK_1,
	S_COMMENT_BLOCK_2,
} state_t;

/**
 * @brief Keyword texts indexed by keyword_t
 *
 * Used to tell keywords apart from identifiers.
 */
const char *const keywords[] = {
	[KW_NONE] = NULL,		[KW_DEF] = "def",		  [KW_DO] = "do",
	[KW_DOUBLE] = "Double", [KW_ELSE] = "else",		  [KW_IF] = "if",
	[KW_IMPORT] = "import", [KW_INT] = "Int",		  [KW_MAIN] = "@main",
	[KW_RETURN] = "return", [KW_STRING] = "String",	  [KW_THEN] = "then",
	[KW_UNIT] = "Unit",		[KW_VAL] = "val",		  [KW_VAR] = "var",
	[KW_WHILE] = "while",	[KW_BOOLEAN] = "Boolean", [KW_EXTENSION] = "extension",
	[KW_FOR] = "for",
};

#define KEYWORD_COUNT (sizeof(keywords) / sizeof(keywords[0]))

/**
 * @brief Look up a word in the keyword table
 *
 * @param word
 * @param keyword
 * @return true if the word is a keyword, false otherwise
 */
bool find_keyword(const char *word, keyword_t *keyword)
{
	for (size_t i = 0; i < KEYWORD_COUNT; i++)
	{
		if (keywords[i] != NULL && strcmp(word, keywords[i]) == 0)
		{
			*keyword = (keyword_t)i;
			return true;
		}
	}

	return false;
}

unsigned scanner_line = 1;			  // Line of the last read character
unsigned scanner_column = 0;		  // Column of the last read character
unsigned scanner_prev_column = 0;	  // Column before the last newline, used by unread_char
bool scanner_first_token = true;	  // The next token is the first one in the file
unsigned scanner_line_indent = 0;	  // Spaces at the start of the current line
bool scanner_line_start = true;		  // Still reading spaces at the start of the line
bool scanner_line_tab = false;		  // A tab was found at the start of the line
unsigned scanner_paren_depth = 0;	  // Number of open parentheses
unsigned scanner_logical_indent = 0;  // Indentation of the current logical line
bool scanner_line_bad_indent = false; // A line inside parentheses is not indented enough

/**
 * @brief Read the next character from the input and update the position
 *
 * @return int The character or EOF
 */
int next_char(void)
{
	int c = getc(stdin);

	if (c == '\n')
	{
		scanner_prev_column = scanner_column;
		scanner_line++;
		scanner_column = 0;
	}
	else if (c != EOF)
	{
		scanner_column++;
	}

	return c;
}

/**
 * @brief Return one character back to the input and restore the position
 *
 * @param c The character to return
 */
void unread_char(int c)
{
	if (c == EOF)
		return;

	ungetc(c, stdin);

	if (c == '\n')
	{
		scanner_line--;
		scanner_column = scanner_prev_column;
	}
	else
	{
		scanner_column--;
	}
}

/**
 * @brief Buffer for the text of the token
 *
 */
typedef struct
{
	char *data;
	size_t len;
	size_t cap;
} buffer_t;

/**
 * @brief Append one character to the buffer
 *
 * @param buf Buffer
 * @param c Character
 * @return true on success, false on allocation failure
 */
bool buf_append(buffer_t *buf, char c)
{
	if (buf->len + 1 >= buf->cap)
	{
		size_t new_cap = buf->cap ? buf->cap * 2 : 32;
		char *tmp = realloc(buf->data, new_cap);

		if (tmp == NULL)
			return false;

		buf->data = tmp;
		buf->cap = new_cap;
	}

	buf->data[buf->len++] = c;
	buf->data[buf->len] = '\0';
	return true;
}

/**
 * @brief Free the buffer and reset it to empty
 *
 * @param buf Buffer
 */
void buf_free(buffer_t *buf)
{
	free(buf->data);
	buf->data = NULL;
	buf->len = 0;
	buf->cap = 0;
}

/**
 * @brief Consume the next character if it matches the expected one
 *
 * @param expected Expected character
 * @return true if the character matched and was consumed, false otherwise
 */
bool next_is(int expected)
{
	int c = next_char();

	if (c == expected)
		return true;

	unread_char(c);
	return false;
}

/**
 * @brief Convert the text of a number to its value
 *
 * @param text Digits of the number
 * @param token Type of number, TOKEN_INT or TOKEN_DOUBLE
 * @return err_t ERR_OK or ERR_SCANNER if the value is out of range
 */
err_t convert_number(const char *text, token_t *token)
{
	errno = 0;

	if (token->type == TOKEN_INT)
	{
		long long value = strtoll(text, NULL, 10);

		if (errno == ERANGE)
		{
			print_err_l("Number is out of range for long long.", ERR_SCANNER, token->line);
			return ERR_SCANNER;
		}

		token->attribute.integer = value;
		return ERR_OK;
	}

	double value = strtod(text, NULL);

	if (errno == ERANGE && (value == HUGE_VAL || value == -HUGE_VAL))
	{
		print_err_l("Number is out of range for double.", ERR_SCANNER, token->line);
		return ERR_SCANNER;
	}

	token->attribute.decimal = value;
	return ERR_OK;
}

/**
 * @brief Free the token buffer, print an error message and return the error
 *
 * @param buf Token buffer
 * @param err Error to return
 * @param message Error message
 * @return err_t The given error
 */
err_t scan_fail(buffer_t *buf, err_t err, const char *message)
{
	buf_free(buf);
	print_err_l(message, err, scanner_line);
	return err;
}

/**
 * @brief Finish an accepted token
 *
 * @param buf Token buffer
 * @param token Accepted token
 * @param comment_eol A multiline block comment was read on the current line
 * @return err_t ERR_OK, ERR_SCANNER if the line starts with a tab,
 *         or ERR_SYNTAX if a token follows a multiline block comment on the same line
 *         or a line inside parentheses is not indented enough
 */
err_t scan_accept(buffer_t *buf, token_t *token, bool comment_eol)
{
	buf_free(buf);

	if (scanner_line_tab && token->type != TOKEN_EOF)
	{
		print_err_l("Tabs cannot be used for indentation.", ERR_SCANNER, token->line);
		free(token->attribute.string);
		token->attribute.string = NULL;
		return ERR_SCANNER;
	}

	if (scanner_line_bad_indent && token->type != TOKEN_EOF)
	{
		print_err_l("A line inside parentheses must be indented more than the start of the logical line.", ERR_SYNTAX,
					token->line);
		free(token->attribute.string);
		token->attribute.string = NULL;
		return ERR_SYNTAX;
	}

	if (comment_eol && token->type != TOKEN_EOF)
	{
		print_err_l("No token can follow a multiline block comment on the same line.", ERR_SYNTAX, token->line);
		free(token->attribute.string);
		token->attribute.string = NULL;
		return ERR_SYNTAX;
	}

	return ERR_OK;
}

/**
 * @brief Convert the number in the buffer and finish the token
 *
 * @param buf Token buffer with the digits of the number
 * @param token Token with its type set to TOKEN_INT or TOKEN_DOUBLE
 * @param comment_eol A multiline block comment was read on the current line
 * @return err_t Error
 */
err_t scan_number(buffer_t *buf, token_t *token, bool comment_eol)
{
	err_t err = convert_number(buf->data, token);
	if (err != ERR_OK)
	{
		buf_free(buf);
		return err;
	}
	return scan_accept(buf, token, comment_eol);
}

/**
 * @brief Move the string from the buffer to the token and finish it
 *
 * @param buf Token buffer with the value of the string
 * @param token Output token
 * @param comment_eol A multiline block comment was read on the current line
 * @return err_t Error
 */
err_t scan_string(buffer_t *buf, token_t *token, bool comment_eol)
{
	if (buf->data == NULL)
	{
		buf->data = calloc(1, 1);
		if (buf->data == NULL)
			return scan_fail(buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");
	}

	token->type = TOKEN_STRING;
	token->attribute.string = buf->data;
	buf->data = NULL;
	return scan_accept(buf, token, comment_eol);
}

/**
 * @brief Read the next token from the input
 *
 * @param token Pointer to where the accepted token should be saved
 * @return err_t Error
 */
err_t scan_next_token(token_t *token)
{
	buffer_t buf = {0};
	state_t state = S_START;
	bool newline_seen = scanner_first_token;
	unsigned depth = 0;
	bool block_newline = false;
	bool comment_eol = false;

	memset(token, 0, sizeof(*token));

	while (true)
	{
		int c = next_char();

		switch (state)
		{
		case S_START:
			if (c == '\n')
			{
				if (scanner_paren_depth == 0)
				{
					newline_seen = true;
					comment_eol = false;
				}

				scanner_line_indent = 0;
				scanner_line_start = true;
				scanner_line_tab = false;
				scanner_line_bad_indent = false;
				break;
			}

			if (c == ' ')
			{
				if (scanner_line_start)
					scanner_line_indent++;
				break;
			}

			if (c == '\t')
			{
				if (scanner_line_start)
					scanner_line_tab = true;
				break;
			}

			if (c == '\r')
				break;

			if (scanner_line_start)
			{
				if (newline_seen)
					scanner_logical_indent = scanner_line_indent;
				else if (scanner_paren_depth > 0 && scanner_line_indent <= scanner_logical_indent)
					scanner_line_bad_indent = true;
			}

			scanner_line_start = false;

			token->line = scanner_line;
			token->starts_logical_line = newline_seen;
			token->indent = scanner_line_indent;
			scanner_first_token = false;

			if (isalpha(c) || c == '@')
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_IDENTIFIER;
				break;
			}

			if (c == '_')
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_UNDERSCORE;
				break;
			}

			if (isdigit(c))
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = (c == '0') ? S_ZERO : S_NUMBER_INT;
				break;
			}

			if (c == '.')
			{
				int next = next_char();
				unread_char(next);

				if (!isdigit(next))
				{
					token->type = TOKEN_DOT;
					return scan_accept(&buf, token, comment_eol);
				}

				if (!buf_append(&buf, '.'))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_DOT;
				break;
			}

			if (c == '"')
			{
				state = S_STRING_START_1;
				break;
			}

			if (c == '/')
			{
				state = S_SLASH;
				break;
			}

			switch (c)
			{
			case EOF:
				token->type = TOKEN_EOF;
				return scan_accept(&buf, token, comment_eol);
			case '(':
				scanner_paren_depth++;
				token->type = TOKEN_OPENING_BRACKET;
				return scan_accept(&buf, token, comment_eol);
			case ')':
				if (scanner_paren_depth > 0)
					scanner_paren_depth--;
				token->type = TOKEN_CLOSING_BRACKET;
				return scan_accept(&buf, token, comment_eol);
			case ',':
				token->type = TOKEN_COMMA;
				return scan_accept(&buf, token, comment_eol);
			case ':':
				token->type = TOKEN_COLON;
				return scan_accept(&buf, token, comment_eol);
			case '+':
				token->type = TOKEN_PLUS;
				return scan_accept(&buf, token, comment_eol);
			case '-':
				token->type = TOKEN_MINUS;
				return scan_accept(&buf, token, comment_eol);
			case '*':
				token->type = TOKEN_ASTERISK;
				return scan_accept(&buf, token, comment_eol);
			case ';':
				token->type = TOKEN_SEMICOLON;
				return scan_accept(&buf, token, comment_eol);
			case '=':
				token->type = next_is('=') ? TOKEN_EQUAL : TOKEN_ASSIGN;
				return scan_accept(&buf, token, comment_eol);
			case '!':
				token->type = next_is('=') ? TOKEN_NOT_EQUAL : TOKEN_NEG;
				return scan_accept(&buf, token, comment_eol);
			case '<':
				if (next_is('='))
					token->type = TOKEN_LESS_EQUAL;
				else if (next_is('-'))
					token->type = TOKEN_FOR_GEN_ASSIGN;
				else
					token->type = TOKEN_LESS;
				return scan_accept(&buf, token, comment_eol);
			case '>':
				token->type = next_is('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER;
				return scan_accept(&buf, token, comment_eol);
			case '&':
				if (!next_is('&'))
					return scan_fail(&buf, ERR_SCANNER, "Expected \"&&\".");
				token->type = TOKEN_AND;
				return scan_accept(&buf, token, comment_eol);
			case '|':
				if (!next_is('|'))
					return scan_fail(&buf, ERR_SCANNER, "Expected \"||\".");
				token->type = TOKEN_OR;
				return scan_accept(&buf, token, comment_eol);
			default:
				return scan_fail(&buf, ERR_SCANNER, "Unknown character.");
			}

		case S_IDENTIFIER:
			if (isalnum(c) || c == '_')
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				break;
			}

			unread_char(c);

			if (find_keyword(buf.data, &token->attribute.keyword))
			{
				token->type = TOKEN_KEYWORD;
				return scan_accept(&buf, token, comment_eol);
			}

			if (buf.data[0] == '@')
				return scan_fail(&buf, ERR_SCANNER, "Unknown annotation, only @main is allowed.");

			if (strcmp(buf.data, "true") == 0 || strcmp(buf.data, "false") == 0)
			{
				token->type = TOKEN_BOOLEAN;
				token->attribute.boolean = (buf.data[0] == 't');
				return scan_accept(&buf, token, comment_eol);
			}

			token->type = TOKEN_IDENTIFIER;
			token->attribute.string = buf.data;
			buf.data = NULL;
			return scan_accept(&buf, token, comment_eol);

		case S_UNDERSCORE:
			if (isalnum(c) || c == '_')
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_IDENTIFIER;
				break;
			}

			unread_char(c);
			token->type = TOKEN_UNDERSCORE;
			return scan_accept(&buf, token, comment_eol);

		case S_ZERO:
			if (c == '.')
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_DOT;
				break;
			}

			if (c == 'e' || c == 'E')
			{
				if (!buf_append(&buf, 'e'))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_EXPONENT;
				break;
			}

			if (isalnum(c) || c == '_')
				return scan_fail(&buf, ERR_SCANNER, "Non-zero number literal cannot start with a zero.");

			unread_char(c);
			token->type = TOKEN_INT;
			return scan_number(&buf, token, comment_eol);

		case S_NUMBER_INT:
			if (isdigit(c))
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				break;
			}

			if (c == '_')
			{
				state = S_NUM_INT_UNDERSCORE;
				break;
			}

			if (c == '.')
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_DOT;
				break;
			}

			if (c == 'e' || c == 'E')
			{
				if (!buf_append(&buf, 'e'))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_EXPONENT;
				break;
			}

			if (isalpha(c))
				return scan_fail(&buf, ERR_SCANNER, "Invalid character in an integer number literal.");

			unread_char(c);
			token->type = TOKEN_INT;
			return scan_number(&buf, token, comment_eol);

		case S_NUM_INT_UNDERSCORE:
			if (c == '_')
				break;

			if (!isdigit(c))
				return scan_fail(&buf, ERR_SCANNER,
								 "An underscore in an integer number literal must be followed by a digit.");

			if (!buf_append(&buf, (char)c))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			state = S_NUMBER_INT;
			break;

		case S_DOT:
			// If the dot is followed by a valid identifier, accept the previous number as an integer
			// This has to be done in order to support extension calls on integers
			if (isalpha(c) || c == '_')
			{
				// Remove the . from the buffer
				buf.data[--buf.len] = '\0';
				unread_char(c);
				unread_char('.');
				token->type = TOKEN_INT;
				return scan_number(&buf, token, comment_eol);
			}

			if (!isdigit(c))
				return scan_fail(&buf, ERR_SCANNER, "Expected a digit after the decimal point.");

			if (!buf_append(&buf, (char)c))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			state = S_NUMBER_DOUBLE;
			break;

		case S_NUMBER_DOUBLE:
			if (isdigit(c))
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				break;
			}

			if (c == '_')
			{
				state = S_NUM_DOUBLE_UNDERSCORE;
				break;
			}

			if (c == 'e' || c == 'E')
			{
				if (!buf_append(&buf, 'e'))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_EXPONENT;
				break;
			}

			if (c == 'd' || c == 'D')
			{
				token->type = TOKEN_DOUBLE;
				return scan_number(&buf, token, comment_eol);
			}

			if (isalpha(c))
				return scan_fail(&buf, ERR_SCANNER, "Invalid character in a double number literal.");

			unread_char(c);
			token->type = TOKEN_DOUBLE;
			return scan_number(&buf, token, comment_eol);

		case S_NUM_DOUBLE_UNDERSCORE:
			if (c == '_')
				break;

			if (!isdigit(c))
				return scan_fail(&buf, ERR_SCANNER,
								 "An underscore in a double number literal must be followed by a digit.");

			if (!buf_append(&buf, (char)c))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			state = S_NUMBER_DOUBLE;
			break;

		case S_EXPONENT:
			if (c == '+' || c == '-')
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				state = S_EXP_SIGN;
				break;
			}
			// fall through

		case S_EXP_SIGN:
			if (!isdigit(c))
				return scan_fail(&buf, ERR_SCANNER, "Expected a digit in the exponent.");

			if (!buf_append(&buf, (char)c))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			state = S_EXP_END;
			break;

		case S_EXP_END:
			if (isdigit(c))
			{
				if (!buf_append(&buf, (char)c))
					return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

				break;
			}

			if (c == '_')
			{
				state = S_EXP_END_UNDERSCORE;
				break;
			}

			if (c == 'd' || c == 'D')
			{
				token->type = TOKEN_DOUBLE;
				return scan_number(&buf, token, comment_eol);
			}

			if (isalpha(c) || c == '.')
				return scan_fail(&buf, ERR_SCANNER, "Invalid character in a double number literal with an exponent.");

			unread_char(c);
			token->type = TOKEN_DOUBLE;
			return scan_number(&buf, token, comment_eol);

		case S_EXP_END_UNDERSCORE:
			if (c == '_')
				break;

			if (!isdigit(c))
				return scan_fail(
					&buf, ERR_SCANNER,
					"An underscore in a double number literal with an exponent must be followed by a digit.");

			if (!buf_append(&buf, (char)c))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			state = S_EXP_END;
			break;

		case S_STRING_START_1:
			if (c == '"')
			{
				state = S_STRING_START_2;
				break;
			}
			// fall through

		case S_STRING:
			if (c == '"')
				return scan_string(&buf, token, comment_eol);

			if (c == '\n' || c == EOF)
				return scan_fail(&buf, ERR_SCANNER, "Unterminated string literal.");

			if (c == '\\')
			{
				state = S_ESCAPE;
				break;
			}

			if (!buf_append(&buf, (char)c))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			state = S_STRING;
			break;

		case S_ESCAPE:
			switch (c)
			{
			case '\\':
			case '"':
				break;
			case 'n':
				c = '\n';
				break;
			case 'r':
				c = '\r';
				break;
			case 't':
				c = '\t';
				break;
			default:
				return scan_fail(&buf, ERR_SCANNER, "Invalid escape sequence.");
			}

			if (!buf_append(&buf, (char)c))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			state = S_STRING;
			break;

		case S_STRING_START_2:
			if (c == '"')
			{
				state = S_MULTILINE_STRING;
				break;
			}

			unread_char(c);
			return scan_string(&buf, token, comment_eol);

		case S_MULTILINE_STRING:
			if (c == EOF)
				return scan_fail(&buf, ERR_SCANNER, "Unterminated multiline string literal.");

			if (c == '"')
			{
				state = S_MULTILINE_END_1;
				break;
			}

			if (!buf_append(&buf, (char)c))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			break;

		case S_MULTILINE_END_1:
			if (c == '"')
			{
				state = S_MULTILINE_END_2;
				break;
			}

			unread_char(c);

			if (!buf_append(&buf, '"'))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			state = S_MULTILINE_STRING;
			break;

		case S_MULTILINE_END_2:
			if (c == '"')
			{
				while (next_is('"'))
				{
					if (!buf_append(&buf, '"'))
						return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");
				}

				return scan_string(&buf, token, comment_eol);
			}

			unread_char(c);

			if (!buf_append(&buf, '"') || !buf_append(&buf, '"'))
				return scan_fail(&buf, ERR_INTERNAL, "Failed to allocate memory in the scanner.");

			state = S_MULTILINE_STRING;
			break;

		case S_SLASH:
			if (c == '/')
			{
				state = S_COMMENT;
				break;
			}

			if (c == '*')
			{
				depth = 1;
				block_newline = false;
				state = S_COMMENT_BLOCK;
				break;
			}

			unread_char(c);
			token->type = TOKEN_DIV;
			return scan_accept(&buf, token, comment_eol);

		case S_COMMENT:
			if (c == '\n' || c == EOF)
			{
				unread_char(c);
				state = S_START;
			}
			break;

		case S_COMMENT_BLOCK:
			if (c == EOF)
				return scan_fail(&buf, ERR_SCANNER, "Unterminated block comment.");

			if (c == '\n')
				block_newline = true;

			if (c == '/')
				state = S_COMMENT_BLOCK_1;
			else if (c == '*')
				state = S_COMMENT_BLOCK_2;

			break;

		case S_COMMENT_BLOCK_1:
			if (c == EOF)
				return scan_fail(&buf, ERR_SCANNER, "Unterminated block comment.");

			if (c == '\n')
				block_newline = true;

			if (c == '*')
			{
				depth++;
				state = S_COMMENT_BLOCK;
			}
			else if (c != '/')
			{
				state = S_COMMENT_BLOCK;
			}
			break;

		case S_COMMENT_BLOCK_2:
			if (c == EOF)
				return scan_fail(&buf, ERR_SCANNER, "Unterminated block comment.");

			if (c == '\n')
				block_newline = true;

			if (c == '/')
			{
				depth--;

				if (depth == 0 && block_newline && scanner_paren_depth == 0)
				{
					newline_seen = true;
					comment_eol = true;
				}

				state = (depth == 0) ? S_START : S_COMMENT_BLOCK;
			}
			else if (c != '*')
			{
				state = S_COMMENT_BLOCK;
			}
			break;

		default:
			break;
		}
	}
}

static bool token_cached;
static token_t token_cache;

/**
 * @brief Check if the token follows the newline rule given by the parser
 *
 * @param token Accepted token
 * @param eol Newline rule
 * @return err_t ERR_OK, or ERR_SYNTAX if the rule is broken
 */
err_t check_eol(const token_t *token, eol_rule_t eol)
{
	if (token->type == TOKEN_EOF)
		return ERR_OK;

	if (eol == EOL_REQUIRED && !token->starts_logical_line)
	{
		print_err_l("Expected a newline before the token.", ERR_SYNTAX, token->line);
		return ERR_SYNTAX;
	}

	if (eol == EOL_FORBIDDEN && token->starts_logical_line)
	{
		print_err_l("Unexpected newline before the token.", ERR_SYNTAX, token->line);
		return ERR_SYNTAX;
	}

	return ERR_OK;
}

err_t get_next_token(token_t *token, eol_rule_t eol)
{
	err_t result = ERR_OK;

	if (token_cached)
	{
		*token = token_cache;
		token_cached = false;
	}
	else
	{
		result = scan_next_token(token);
		if (result)
			return result;
	}

	result = check_eol(token, eol);
	if (result)
	{
		free(token->attribute.string);
		token->attribute.string = NULL;
	}

	return result;
}

err_t peek_next_token(token_t *token, eol_rule_t eol)
{
	if (!token_cached)
	{
		err_t result = scan_next_token(&token_cache);
		if (result)
			return result;
		token_cached = true;
	}

	*token = token_cache;
	return check_eol(token, eol);
}
