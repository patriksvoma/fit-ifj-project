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

	return ERR_SCANNER;
}
