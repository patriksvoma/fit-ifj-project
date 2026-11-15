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
err_t ext_def(parser_data_t *pdata);
err_t ext_def_list(parser_data_t *pdata);
err_t func_annotation(parser_data_t *pdata);
err_t func_def(parser_data_t *pdata);
err_t func_params(parser_data_t *pdata);
err_t param_first(parser_data_t *pdata);
err_t param_list(parser_data_t *pdata);
err_t param(parser_data_t *pdata);
err_t return_type(parser_data_t *pdata);
err_t block_start(parser_data_t *pdata);
err_t block(parser_data_t *pdata);
err_t type(parser_data_t *pdata);
err_t statement(parser_data_t *pdata);
err_t expression_opt(parser_data_t *pdata);
err_t else_opt(parser_data_t *pdata);
err_t else_if_opt(parser_data_t *pdata);
err_t for_generator(parser_data_t *pdata);
err_t for_by(parser_data_t *pdata);
err_t for_guard(parser_data_t *pdata);
err_t val_var_def(parser_data_t *pdata);
err_t expression(parser_data_t *pdata);
err_t expression_seeded(parser_data_t *pdata, token_t *seed_token);

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
	err_t result;

	// @main or def, go to <func-annotation>
	if (pdata->token.type == TOKEN_KEYWORD &&
		(pdata->token.attribute.keyword == KW_MAIN || pdata->token.attribute.keyword == KW_DEF))
	{
		result = func_annotation(pdata);
		if (result)
			return result;

		return func_list(pdata);
	}
	// extension
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_EXTENSION)
	{
		// <ext-def>
		result = ext_def(pdata);
		if (result)
			return result;

		// <func-list>
		return func_list(pdata);
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
 * @brief Rule for extension definitions | <ext-def>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t ext_def(parser_data_t *pdata)
{
	err_t result;

	// extension
	if (pdata->token.type != TOKEN_KEYWORD || pdata->token.attribute.keyword != KW_EXTENSION)
	{
		print_err_l("Expected a type extension block declaration", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	// (
	if (pdata->token.type != TOKEN_OPENING_BRACKET)
	{
		print_err_l("Missing \"(\" after an extension keyword.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&pdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	// <param>
	result = param(pdata);
	if (result)
		return result;

	// )
	if (pdata->token.type != TOKEN_CLOSING_BRACKET)
	{
		print_err_l("Missing \")\" after an extension type declaration.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&pdata->token, EOL_REQUIRED);
	if (result)
		return result;

	// <func-def>
	result = func_def(pdata);
	if (result)
		return result;

	// <ext-def-list>
	return ext_def_list(pdata);
}

/**
 * @brief Rule for lists of extension function declarations | <ext-def-list>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t ext_def_list(parser_data_t *pdata)
{
	// def
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_DEF)
	{
		err_t result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <func-def>
		result = func_def(pdata);
		if (result)
			return result;

		// <ext-def-list>
		return ext_def_list(pdata);
	}

	// Epsilon rule
	return ERR_OK;
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
		// <param>
		result = param(pdata);
		if (result)
			return result;

		// <param-list>
		return param_list(pdata);
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
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		result = param(pdata);
		if (result)
			return result;

		return param_list(pdata);
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
		print_err_l("Expected an identifier.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&pdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	// :
	if (pdata->token.type != TOKEN_COLON)
	{
		print_err_l("Expected a \":\" separator between identifier and type.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&pdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	// Type
	return type(pdata);
}

/**
 * @brief Function return type definition | <return-type>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t return_type(parser_data_t *pdata)
{
	err_t result;

	// =, go to <block-start>
	if (pdata->token.type == TOKEN_ASSIGN)
	{
		result = get_next_token(&pdata->token, EOL_REQUIRED);
		if (result)
			return result;

		return block_start(pdata);
	}
	// :
	else if (pdata->token.type == TOKEN_COLON)
	{
		// <type>
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		result = type(pdata);
		if (result)
			return result;

		// =
		if (pdata->token.type != TOKEN_ASSIGN)
		{
			print_err_l("Expected a block start delimiter (=).", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}

		// <block-start>
		return block_start(pdata);
	}

	print_err_l("Expected a return type definiton, or a block start delimiter (=).", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Statement block start | <block-start>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t block_start(parser_data_t *pdata)
{
	// <statement>
	err_t result = statement(pdata);
	if (result)
		return result;

	// <block>
	return block(pdata);
}

/**
 * @brief Statement block | <block>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t block(parser_data_t *pdata)
{
	err_t result = statement(pdata);
	if (result)
		return result;

	// TODO: End blocks by checking indentation
	return block(pdata);
}

/**
 * @brief Type parsing rule | <type>
 *
 * @param pdata Parser data
 * @return err_t Erroc code
 */
err_t type(parser_data_t *pdata)
{
	bool valid_keyword = false;

	if (pdata->token.type == TOKEN_KEYWORD)
	{
		switch (pdata->token.attribute.keyword)
		{
		case KW_INT:
			valid_keyword = true;
			break;
		case KW_DOUBLE:
			valid_keyword = true;
			break;
		case KW_STRING:
			valid_keyword = true;
			break;
		case KW_UNIT:
			valid_keyword = true;
			break;
		case KW_BOOLEAN:
			valid_keyword = true;
			break;
		default:
			break;
		}
	}

	if (valid_keyword)
	{
		// FIXME: The end of line cannot be decided here, as the <type> rule can be used both inside and outside
		// parenthesis.
		// The fix is probably to hold a "in_parenthesis" value in the parser data and decide based on that.
		return get_next_token(&pdata->token, EOL_OPTIONAL);
	}
	else
	{
		print_err_l("Expected a valid type (Int, Double, String, Unit, Boolean).", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}
}

/**
 * @brief Statements | <statement>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t statement(parser_data_t *pdata)
{
	// TODO: Remove suppression
	// cppcheck-suppress-begin knownConditionTrueFalse
	err_t result;

	// val
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_VAL)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// id
		if (pdata->token.type != TOKEN_IDENTIFIER)
		{
			print_err_l("Expected an identifier in constant declaration.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <val-var-def>
		return val_var_def(pdata);
	}
	// var
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_VAR)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// id
		if (pdata->token.type != TOKEN_IDENTIFIER)
		{
			print_err_l("Expected an identifier in variable declaration.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <val-var-def>
		return val_var_def(pdata);
	}
	// if
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_IF)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		result = expression(pdata);
		if (result)
			return result;

		// then
		if (pdata->token.type != TOKEN_KEYWORD || pdata->token.attribute.keyword != KW_THEN)
		{
			print_err_l("Expected a \"then\" keyword after expression.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <block-start>
		result = block_start(pdata);
		if (result)
			return result;

		// <else-opt>
		return else_opt(pdata);
	}
	// while
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_WHILE)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		result = expression(pdata);
		if (result)
			return result;

		// do
		if (pdata->token.type != TOKEN_KEYWORD || pdata->token.attribute.keyword != KW_DO)
		{
			print_err_l("Expected a \"do\" keyword after expression.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&pdata->token, EOL_REQUIRED);
		if (result)
			return result;

		// <block-start>
		return block_start(pdata);
	}
	// for
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_FOR)
	{
		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// identifier
		if (pdata->token.type != TOKEN_IDENTIFIER)
		{
			print_err_l("Expected an iterator identifier after \"for\".", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <-
		if (pdata->token.type != TOKEN_FOR_GEN_ASSIGN)
		{
			print_err_l("Expected a generator variable assignment operator (<-).", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		result = expression(pdata);
		if (result)
			return result;

		// <for-generator>
		result = for_generator(pdata);
		if (result)
			return result;

		// <for-guard>
		result = for_guard(pdata);
		if (result)
			return result;

		// do
		if (pdata->token.type != TOKEN_KEYWORD || pdata->token.attribute.keyword != KW_DO)
		{
			print_err_l("Expected the \"do\" keyword after a for guard.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&pdata->token, EOL_REQUIRED);
		if (result)
			return result;

		// <block-start>
		return block_start(pdata);
	}
	// return
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_RETURN)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		return expression_opt(pdata);
	}
	// identifier
	else if (pdata->token.type == TOKEN_IDENTIFIER)
	{
		// A lookahead needs to be performed here in order to
		// tell between the rules:
		// id = <expression>
		// <expression>

		token_t id_token = pdata->token;

		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// id = <expression>
		if (pdata->token.type == TOKEN_ASSIGN)
		{
			result = get_next_token(&pdata->token, EOL_FORBIDDEN);
			if (result)
				return result;

			return expression(pdata);
		}

		// <expression>
		// The expression parser needs to have the id token seeded before the token in pdata
		return expression_seeded(pdata, &id_token);
	}

	// <expression>
	return expression(pdata);
	// TODO: Remove suppression
	// cppcheck-suppress-end knownConditionTrueFalse
}

err_t expression_opt(parser_data_t *pdata)
{
	// TODO:
	(void)pdata;
	return ERR_SYNTAX;
}

/**
 * @brief Optional else branch of an if, later can be made into an else if branch | <else-opt>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t else_opt(parser_data_t *pdata)
{
	// else
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_ELSE)
	{
		err_t result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <else-if-opt>
		return else_if_opt(pdata);
	}

	// Epsilon rule
	return ERR_OK;
}

/**
 * @brief Else, or else if branch of an if statement | <else-if-opt>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t else_if_opt(parser_data_t *pdata)
{
	// TODO: Remove suppression
	// cppcheck-suppress-begin knownConditionTrueFalse
	// if (makes for an else if with the previous rule)
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_IF)
	{
		err_t result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <expression>
		result = expression(pdata);
		if (result)
			return result;

		// then
		if (pdata->token.type != TOKEN_KEYWORD || pdata->token.attribute.keyword != KW_THEN)
		{
			print_err_l("Expected a \"then\" keyword after expression.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <block-start>
		result = block_start(pdata);
		if (result)
			return result;

		// <else-opt>
		return else_opt(pdata);
	}

	// <block_start>
	return block_start(pdata);
	// TODO: Remove suppression
	// cppcheck-suppress-end knownConditionTrueFalse
}

/**
 * @brief For statement generator | <for-generator>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t for_generator(parser_data_t *pdata)
{
	// TODO: Remove suppression
	// cppcheck-suppress-begin knownConditionTrueFalse
	// identifier
	if (pdata->token.type == TOKEN_IDENTIFIER)
	{
		err_t result;

		if (!strcmp(pdata->token.attribute.string, "to"))
		{
			result = get_next_token(&pdata->token, EOL_FORBIDDEN);
			if (result)
				return result;

			// <expression>
			result = expression(pdata);
			if (result)
				return result;

			// <for-by>
			return for_by(pdata);
		}
		else if (!strcmp(pdata->token.attribute.string, "until"))
		{
			result = get_next_token(&pdata->token, EOL_FORBIDDEN);
			if (result)
				return result;

			// <expression>
			return expression(pdata);
		}
	}

	print_err_l("Expected \"to\" or \"until\" as a for statement generator.", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
	// TODO: Remove suppression
	// cppcheck-suppress-end knownConditionTrueFalse
}

/**
 * @brief For statement generator "by" | <for-by>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t for_by(parser_data_t *pdata)
{
	// by
	if (pdata->token.type == TOKEN_IDENTIFIER && !strcmp(pdata->token.attribute.string, "by"))
	{
		err_t result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		return expression(pdata);
	}

	// Epsilon rule
	return ERR_OK;
}

/**
 * @brief For statement guard | <for-guard>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t for_guard(parser_data_t *pdata)
{
	// TODO: Remove suppression
	// cppcheck-suppress-begin knownConditionTrueFalse
	// if
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_IF)
	{
		err_t result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		result = expression(pdata);
		if (result)
			return result;

		// <for-guard>
		return for_guard(pdata);
	}

	// Epsilon rule
	return ERR_OK;
	// TODO: Remove suppression
	// cppcheck-suppress-end knownConditionTrueFalse
}

/**
 * @brief Val or var definition
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t val_var_def(parser_data_t *pdata)
{
	err_t result;

	// :
	if (pdata->token.type == TOKEN_COLON)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <type>
		result = type(pdata);
		if (result)
			return result;

		// =
		if (pdata->token.type != TOKEN_ASSIGN)
		{
			print_err_l("Expected an assignment operator after type declaration.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression
		return expression(pdata);
	}

	// =
	if (pdata->token.type == TOKEN_ASSIGN)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		return expression(pdata);
	}

	print_err_l("Expected a type declaration or an assignment operator after identifier", ERR_SYNTAX,
				pdata->token.line);
	return ERR_SYNTAX;
}

err_t expression(parser_data_t *pdata)
{
	// TODO:
	(void)pdata;
	return ERR_SYNTAX;
}

err_t expression_seeded(parser_data_t *pdata, token_t *seed_token)
{
	// TODO:
	(void)pdata;
	(void)seed_token;
	return ERR_SYNTAX;
}

err_t parse()
{
	parser_data_t pdata = {};

	err_t result = prologue(&pdata);

	return result;
}
