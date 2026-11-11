/**
 * @file parser.c
 * @brief Syntax analyzer for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "parser.h"
#include "error.h"
#include <string.h>

// Forward declarations of parser rules
err_t prologue(parser_data_t *pdata);
err_t func_list(parser_data_t *pdata);
err_t func_annotation(parser_data_t *pdata);
err_t ext_def(parser_data_t *pdata);

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
	{
		print_err_l("Invalid token. Expected the \"import\" keyword.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}

	// Identifier ifj26
	result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	if (pdata->token.type != TOKEN_IDENTIFIER)
	{
		print_err_l("Invalid token. Expected an identifier.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}
	if (strcmp(pdata->token.attribute.string, "ifj26"))
	{
		print_err_l("Invalid token. Expected the \"ifj26\" identifier.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}

	// Dot
	result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	if (pdata->token.type != TOKEN_DOT)
	{
		print_err_l("Invalid token. Expected a \".\".", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}

	// Asterisk
	result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	if (pdata->token.type != TOKEN_ASTERISK)
	{
		print_err_l("Invalid token. Expected a \"*\".", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}

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
	err_t result = get_next_token(&pdata->token, EOL_REQUIRED);
	if (result)
		return result;

	// Go to <func-annotation>
	if (pdata->token.type == TOKEN_KEYWORD &&
		(pdata->token.attribute.keyword == KW_MAIN || pdata->token.attribute.keyword == KW_DEF))
	{
		return func_annotation(pdata);
	}
	// Go to <ext-def>
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_EXTENSION)
	{
		return ext_def(pdata);
	}
	// If EOF is received, end the function list
	else if (pdata->token.type == TOKEN_EOF)
	{
		return ERR_OK;
	}

	print_err_l("Expected a function or extension declaration.", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Rule for the main function annotiation | <func-annotation>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t func_annotation(parser_data_t *pdata)
{
	// TODO:
	(void)pdata;
	return ERR_SYNTAX;
}

/**
 * @brief Rule for extension definitions | <ext-def>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t ext_def(parser_data_t *pdata)
{
	// TODO:
	(void)pdata;
	return ERR_SYNTAX;
}

err_t parse()
{
	parser_data_t pdata = {};

	err_t result = prologue(&pdata);

	return result;
}
