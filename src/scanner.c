/**
 * @file scanner.c
 * @brief Scanner module for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "scanner.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


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
	[KW_NONE] = NULL,
	[KW_DEF] = "def",
	[KW_DO] = "do",
	[KW_DOUBLE] = "Double",
	[KW_ELSE] = "else",
	[KW_IF] = "if",
	[KW_IMPORT] = "import",
	[KW_INT] = "Int",
	[KW_MAIN] = "@main",
	[KW_RETURN] = "return",
	[KW_STRING] = "String",
	[KW_THEN] = "then",
	[KW_UNIT] = "Unit",
	[KW_VAL] = "val",
	[KW_VAR] = "var",
	[KW_WHILE] = "while",
	[KW_BOOLEAN] = "Boolean",
	[KW_EXTENSION] = "extension",
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

unsigned scanner_line = 1;        // Line of the last read character
unsigned scanner_column = 0;      // Column of the last read character
unsigned scanner_prev_column = 0; // Column before the last newline, used by unread_char

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
 * @brief Get the next token object
 *
 * @param token
 * @param eol
 * @return err_t
 */
err_t get_next_token(token_t *token, eol_rule_t eol)
{
	buffer_t buf = {0};
	state_t state = S_START;
	bool newline_seen = false;
	int c;

	memset(token, 0, sizeof(*token));

	while (true)
	{
		c = next_char();

		switch (state)
		{
		case S_START:
			if (c == '\n')
			{
				newline_seen = true;
				break;
			}
			if (c == ' ' || c == '\r')
				break;

			token->line = scanner_line;
			token->starts_logical_line = newline_seen;

			switch (c)
			{
			case EOF: token->type = TOKEN_EOF; goto accept;
			case '(': token->type = TOKEN_OPENING_BRACKET; goto accept;
			case ')': token->type = TOKEN_CLOSING_BRACKET; goto accept;
			case ',': token->type = TOKEN_COMMA; goto accept;
			case ':': token->type = TOKEN_COLON; goto accept;
			case '+': token->type = TOKEN_PLUS; goto accept;
			case '-': token->type = TOKEN_MINUS; goto accept;
			case '*': token->type = TOKEN_ASTERISK; goto accept;
			default:
				goto fail;
			}

		default:
			break;
		}
	}

accept:
	buf_free(&buf);
	(void)eol; // TODO: check EOL
	return ERR_OK;

fail:
	buf_free(&buf);
	return ERR_SCANNER;
}
