/**
 * @file parser.c
 * @brief Syntax analyzer for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "parser.h"
#include <string.h>

// Forward declarations of parser rules
err_t prologue(parser_data_t *pdata);
err_t func_list(parser_data_t *pdata);

/**
 * @brief The entry rule of syntax analysis | <prologue>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t prologue(parser_data_t *pdata)
{
	// Keyword import
	err_t result = get_next_token(&pdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	if (pdata->token.type != TOKEN_KEYWORD || pdata->token.attribute.keyword != KW_IMPORT)
		return ERR_SYNTAX;

	// Identifier ifj26
	result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	if (pdata->token.type != TOKEN_IDENTIFIER)
		return ERR_SYNTAX;
	if (strcmp(pdata->token.attribute.string, "ifj26"))
		return ERR_SYNTAX;

	// Dot
	result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	if (pdata->token.type != TOKEN_DOT)
		return ERR_SYNTAX;

	// Asterisk
	result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	if (pdata->token.type != TOKEN_ASTERISK)
		return ERR_SYNTAX;

	// <func-list>
	return func_list(pdata);
}

/**
 * @brief List of global functions or extension definitons | <func-list>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t func_list(parser_data_t *pdata)
{
	(void)pdata;

	return ERR_SYNTAX;
}

err_t parse()
{
	parser_data_t pdata = {};

	err_t result = prologue(&pdata);

	return result;
}
