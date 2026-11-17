/**
 * @file scanner.h
 * @brief Scanner module for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "scanner.h"

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

/**
 * @brief Get the next token object
 *
 * @param token
 * @param eol
 * @return err_t
 */
err_t get_next_token(token_t *token, eol_rule_t eol)
{
	// TODO: This placeholder code is here just to pass code checks
	token->attribute.integer = 0;
	(void)eol;
	(void)find_keyword;

	return ERR_SCANNER;
}
