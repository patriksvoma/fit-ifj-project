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
err_t func_def(parser_data_t *pdata);
err_t func_params(parser_data_t *pdata);
err_t param_first(parser_data_t *pdata);
err_t param_list(parser_data_t *pdata);
err_t param(parser_data_t *pdata);
err_t return_type(parser_data_t *pdata);
err_t type(parser_data_t *pdata);

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
	result = get_next_token(&pdata->token, EOL_REQUIRED);
	if (result)
		return result;
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
	// @main or def, go to <func-annotation>
	if (pdata->token.type == TOKEN_KEYWORD &&
		(pdata->token.attribute.keyword == KW_MAIN || pdata->token.attribute.keyword == KW_DEF))
	{
		err_t result = func_annotation(pdata);
		if (result)
			return result;

		return func_list(pdata);
	}
	// extension, go to <ext-def>
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
	// @main, go to <func-def>
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_MAIN)
	{
		err_t result;

		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;
		return func_def(pdata);
	}
	// def, go to <func-def>
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_DEF)
	{
		return func_def(pdata);
	}

	print_err_l("Expeced a function declaration.", ERR_SYNTAX, pdata->token.line);
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

/**
 * @brief Rule for function declarations | <func-def>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t func_def(parser_data_t *pdata)
{
	// def
	if (pdata->token.type != TOKEN_KEYWORD || pdata->token.attribute.keyword != KW_DEF)
	{
		print_err_l("Expected a function declaration.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}

	// Function identifier
	err_t result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;
	if (pdata->token.type != TOKEN_IDENTIFIER)
	{
		print_err_l("Expected an identifier in a function declaration.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}

	result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	// <func-params>
	result = func_params(pdata);
	if (result)
		return result;

	// <return-type>
	return return_type(pdata);
}

/**
 * @brief Rule for the start of function parameter declaration | <func-params>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t func_params(parser_data_t *pdata)
{
	// (, go to <param-first>
	if (pdata->token.type == TOKEN_OPENING_BRACKET)
	{
		err_t result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		return param_first(pdata);
	}

	// Epsilon rule
	return ERR_OK;
}

/**
 * @brief First parameter of a parameter list rule | <param-first>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t param_first(parser_data_t *pdata)
{
	err_t result;

	// ), go to <func-params>
	if (pdata->token.type == TOKEN_CLOSING_BRACKET)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		return func_params(pdata);
	}
	// Identifier
	else if (pdata->token.type == TOKEN_IDENTIFIER)
	{
		// TODO: Remove suppression check
		// cppcheck-suppress-begin knownConditionTrueFalse

		// <param>
		result = param(pdata);
		if (result)
			return result;

		// <param-list>
		return param_list(pdata);
		// cppcheck-suppress-end knownConditionTrueFalse
	}

	print_err_l("Expected a parameter or an end of the parameter list.", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Continuation of a parameter list | <param-list>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t param_list(parser_data_t *pdata)
{
	err_t result;

	// ), go to <func-params>
	if (pdata->token.type == TOKEN_CLOSING_BRACKET)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		return func_params(pdata);
	}
	// ","
	else if (pdata->token.type == TOKEN_COMMA)
	{
		// TODO: Remove suppression
		// cppcheck-suppress-begin knownConditionTrueFalse
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		result = param(pdata);
		if (result)
			return result;

		return param_list(pdata);
		// cppcheck-suppress-end knownConditionTrueFalse
	}

	print_err_l("Expected a comma or and end of a parameter list.", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Function parameter definition rule | <param>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t param(parser_data_t *pdata)
{
	err_t result;

	// Identifier
	if (pdata->token.type != TOKEN_IDENTIFIER)
	{
		print_err_l("Expected a parameter identifier.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&pdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	// :
	if (pdata->token.type != TOKEN_COLON)
	{
		print_err_l("Expected a \":\" separator between parameter identifier and type.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&pdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	// Type
	return type(pdata);
}

err_t return_type(parser_data_t *pdata)
{
	// TODO:
	(void)pdata;
	return ERR_SYNTAX;
}

err_t type(parser_data_t *pdata)
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
