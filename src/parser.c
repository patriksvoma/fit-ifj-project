/**
 * @file parser.c
 * @brief Recursive (top-down) part of the syntax analyzer for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "parser.h"
#include "error.h"
#include "expression.h"
#include <string.h>

// Forward declarations of parser rules
err_t prologue(parser_data_t *pdata, ast_node_t *program_node);
err_t func_list(parser_data_t *pdata, ast_node_t *program_node);
err_t ext_def(parser_data_t *pdata);
err_t ext_def_list(parser_data_t *pdata, unsigned list_indent);
err_t func_annotation(parser_data_t *pdata, ast_node_t *program_node);
err_t func_def(parser_data_t *pdata, ast_node_t *func_def_node);
err_t func_params(parser_data_t *pdata);
err_t param_first(parser_data_t *pdata);
err_t param_list(parser_data_t *pdata);
err_t param(parser_data_t *pdata);
err_t return_type(parser_data_t *pdata);
err_t block_start(parser_data_t *pdata, unsigned header_indent_level);
err_t block(parser_data_t *pdata, unsigned block_indent_level);
err_t type(parser_data_t *pdata);
err_t statement(parser_data_t *pdata);
err_t expression_opt(parser_data_t *pdata, ast_node_t **return_value);
err_t else_opt(parser_data_t *pdata, unsigned header_indent_level);
err_t else_if_opt(parser_data_t *pdata, unsigned header_indent_level);
err_t for_generator(parser_data_t *pdata);
err_t for_by(parser_data_t *pdata);
err_t for_guard(parser_data_t *pdata);
err_t val_var_def(parser_data_t *pdata);
err_t expression(parser_data_t *pdata, ast_node_t **return_node);
err_t expression_seeded(parser_data_t *pdata, ast_node_t **return_node, token_t *seed_token);

/**
 * @brief The entry rule of syntax analysis | <prologue>
 *
 * @param pdata Parser data
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t prologue(parser_data_t *pdata, ast_node_t *program_node)
{
	// Keyword import
	err_t result = get_next_token(&pdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	if (pdata->token.indent != 0)
	{
		print_err_l("The program prologue must be at indentation level 0.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}

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

	return func_list(pdata, program_node);
}

/**
 * @brief List of global functions or extension definitons | <func-list>
 *
 * @param pdata Parser data
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t func_list(parser_data_t *pdata, ast_node_t *program_node)
{
	err_t result;

	// @main or def, go to <func-annotation>
	if (pdata->token.type == TOKEN_KEYWORD &&
		(pdata->token.attribute.keyword == KW_MAIN || pdata->token.attribute.keyword == KW_DEF))
	{
		result = func_annotation(pdata, program_node);
		if (result)
			return result;

		return func_list(pdata, program_node);
	}
	// extension
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_EXTENSION)
	{
		// <ext-def>
		result = ext_def(pdata);
		if (result)
			return result;

		// <func-list>
		return func_list(pdata, program_node);
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

	if (pdata->token.indent != 0)
	{
		print_err_l("Extension block header must be at indent level 0.", ERR_SYNTAX, pdata->token.line);
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

	unsigned extension_list_indent = pdata->token.indent;
	if (extension_list_indent == 0)
	{
		print_err_l("Extensions methods must be indented.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}

	// <func-def>
	// TODO: Pass the correct AST node
	result = func_def(pdata, NULL);
	if (result)
		return result;

	// <ext-def-list>
	return ext_def_list(pdata, extension_list_indent);
}

/**
 * @brief Rule for lists of extension function declarations | <ext-def-list>
 *
 * @param pdata Parser data
 * @param list_indent Indentation level of the first extension definition, which all the others have to match
 * @return err_t Error code
 */
err_t ext_def_list(parser_data_t *pdata, unsigned list_indent)
{
	if (pdata->token.indent > list_indent)
	{
		print_err_l("All extension method definitions must be at the same indent level.", ERR_SYNTAX,
					pdata->token.line);
		return ERR_SYNTAX;
	}
	else if (pdata->token.indent < list_indent)
		return ERR_OK;

	// def
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_DEF)
	{
		err_t result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <func-def>
		// TODO: Pass the correct AST node
		result = func_def(pdata, NULL);
		if (result)
			return result;

		// <ext-def-list>
		return ext_def_list(pdata, list_indent);
	}

	// Epsilon rule
	return ERR_OK;
}

/**
 * @brief Rule for the main function annotiation | <func-annotation>
 *
 * @param pdata Parser data
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t func_annotation(parser_data_t *pdata, ast_node_t *program_node)
{
	ast_node_t *func_node;

	// @main, go to <func-def>
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_MAIN)
	{
		if (pdata->token.indent != 0)
		{
			print_err_l("Function annotation must be at indent level 0.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}

		err_t result;

		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// TODO: Create an entry in the symbol table
		func_node = ast_make_func_def(true);
		ast_array_append(program_node, func_node);

		return func_def(pdata, func_node);
	}
	// def, go to <func-def>
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_DEF)
	{
		if (pdata->token.indent != 0)
		{
			print_err_l("Function definition must be at indent level 0.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}

		// TODO: Create an entry in the symbol table
		func_node = ast_make_func_def(true);
		ast_array_append(program_node, func_node);

		return func_def(pdata, func_node);
	}

	print_err_l("Expeced a function declaration.", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Rule for function declarations | <func-def>
 *
 * @param pdata Parser data
 * @param func_def_node AST function definition node
 * @return err_t Error code
 */
err_t func_def(parser_data_t *pdata, ast_node_t *func_def_node)
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
	// Save the identificator
	// TODO: Check if the identificator already exists
	// TODO: Save into the symbol table
	result = ast_func_def_set_id(func_def_node, pdata->token.attribute.string);
	if (result)
		return result;

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

	unsigned header_indentation = pdata->token.indent;

	// =, go to <block-start>
	if (pdata->token.type == TOKEN_ASSIGN)
	{
		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		return block_start(pdata, header_indentation);
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
		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <block-start>
		return block_start(pdata, header_indentation);
	}

	print_err_l("Expected a return type definiton, or a block start delimiter (=).", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Statement block start | <block-start>
 *
 * @param pdata Parser data
 * @param header_indent_level Indentation level of the block header
 * @return err_t Error code
 */
err_t block_start(parser_data_t *pdata, unsigned header_indent_level)
{
	bool is_inline = !pdata->token.starts_logical_line;

	// If the first statement is on the same line as the header, there is only one statement allowed
	if (is_inline)
	{
		return statement(pdata);
	}

	unsigned statement_indent_level = pdata->token.indent;
	if (pdata->token.starts_logical_line && statement_indent_level <= header_indent_level)
	{
		print_err_l("Start of a block must be on a higher indentation level than the header.", ERR_SYNTAX,
					pdata->token.line);
		return ERR_SYNTAX;
	}

	// <statement>
	err_t result = statement(pdata);
	if (result)
		return result;

	// <block>
	return block(pdata, statement_indent_level);
}

/**
 * @brief Statement block | <block>
 *
 * @param pdata Parser data
 * @param block_indent_level Indentation level every statement in the block must match
 * @return err_t Error code
 */
err_t block(parser_data_t *pdata, unsigned block_indent_level)
{
	while (pdata->token.type == TOKEN_SEMICOLON)
	{
		err_t result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;
	}

	if (pdata->token.type == TOKEN_EOF)
		return ERR_OK;

	unsigned statement_indent_level = pdata->token.indent;
	// Invalid
	if (statement_indent_level > block_indent_level)
	{
		print_err_l("All statements within a block must be on the same indentation level.", ERR_SYNTAX,
					pdata->token.line);
		return ERR_SYNTAX;
	}
	// End of the block
	else if (statement_indent_level < block_indent_level)
		return ERR_OK;

	err_t result = statement(pdata);
	if (result)
		return result;

	// TODO: End blocks by checking indentation
	return block(pdata, block_indent_level);
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
	err_t result;
	unsigned statement_indent_level = pdata->token.indent;

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
		ast_node_t *condition_node;
		result = expression(pdata, &condition_node);
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
		result = block_start(pdata, statement_indent_level);
		if (result)
			return result;

		// <else-opt>
		return else_opt(pdata, statement_indent_level);
	}
	// while
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_WHILE)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *condition_node;
		result = expression(pdata, &condition_node);
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
		return block_start(pdata, statement_indent_level);
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
		ast_node_t *iterator_node;
		result = expression(pdata, &iterator_node);
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
		return block_start(pdata, statement_indent_level);
	}
	// return
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_RETURN)
	{
		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <expression>
		ast_node_t *return_value_node;
		return expression_opt(pdata, &return_value_node);
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

			ast_node_t *assigned_value_node;
			return expression(pdata, &assigned_value_node);
		}

		// <expression>
		// The expression parser needs to have the id token seeded before the token in pdata
		ast_node_t *expression_node;
		return expression_seeded(pdata, &expression_node, &id_token);
	}

	// <expression>
	ast_node_t *expression_node;
	return expression(pdata, &expression_node);
}

/**
 * @brief Parses the statement in a return call. If there is no statement returns a node with the Unit type
 *
 * @param pdata Parser data
 * @param return_node Address to which the address of the created node will be saved
 * @return err_t Erroc code
 */
err_t expression_opt(parser_data_t *pdata, ast_node_t **return_node)
{
	// If the token after return is on another line, the return is empty
	if (pdata->token.starts_logical_line || pdata->token.type == TOKEN_EOF)
	{
		literal_t lit;
		lit.literal_type = TYPE_UNIT;

		*return_node = ast_make_literal(&lit);
		if (*return_node == NULL)
		{
			print_err_l("Failed to create an AST node inside expression_opt()", ERR_INTERNAL, pdata->token.line);
			return ERR_INTERNAL;
		}

		return ERR_OK;
	}

	return expression(pdata, return_node);
}

/**
 * @brief Optional else branch of an if, later can be made into an else if branch | <else-opt>
 *
 * @param pdata Parser data
 * @param header_indent_level Indentation level of the if statement headers (if, else if, else)
 * @return err_t Error code
 */
err_t else_opt(parser_data_t *pdata, unsigned header_indent_level)
{
	// else
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_ELSE &&
		pdata->token.indent == header_indent_level)
	{
		err_t result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <else-if-opt>
		return else_if_opt(pdata, header_indent_level);
	}

	// Epsilon rule
	return ERR_OK;
}

/**
 * @brief Else, or else if branch of an if statement | <else-if-opt>
 *
 * @param pdata Parser data
 * @param header_indent_level Indentation level of the if statement headers (if, else if, else)
 * @return err_t Error code
 */
err_t else_if_opt(parser_data_t *pdata, unsigned header_indent_level)
{
	// if (makes for an else if with the previous rule)
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_IF)
	{
		err_t result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <expression>
		ast_node_t *condition_node;
		result = expression(pdata, &condition_node);
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
		result = block_start(pdata, header_indent_level);
		if (result)
			return result;

		// <else-opt>
		return else_opt(pdata, header_indent_level);
	}

	// <block_start>
	return block_start(pdata, header_indent_level);
}

/**
 * @brief For statement generator | <for-generator>
 *
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t for_generator(parser_data_t *pdata)
{
	// identifier
	if (pdata->token.type == TOKEN_IDENTIFIER)
	{
		err_t result;

		// to
		if (!strcmp(pdata->token.attribute.string, "to"))
		{
			result = get_next_token(&pdata->token, EOL_FORBIDDEN);
			if (result)
				return result;

			// <expression>
			ast_node_t *to_node;
			result = expression(pdata, &to_node);
			if (result)
				return result;

			// <for-by>
			return for_by(pdata);
		}
		// until
		else if (!strcmp(pdata->token.attribute.string, "until"))
		{
			result = get_next_token(&pdata->token, EOL_FORBIDDEN);
			if (result)
				return result;

			// <expression>
			ast_node_t *until_node;
			return expression(pdata, &until_node);
		}
	}

	print_err_l("Expected \"to\" or \"until\" as a for statement generator.", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
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
		ast_node_t *by_node;
		return expression(pdata, &by_node);
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
	// if
	if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_IF)
	{
		err_t result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *condition_node;
		result = expression(pdata, &condition_node);
		if (result)
			return result;

		// <for-guard>
		return for_guard(pdata);
	}

	// Epsilon rule
	return ERR_OK;
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
		ast_node_t *assigned_value_node;
		return expression(pdata, &assigned_value_node);
	}

	// =
	if (pdata->token.type == TOKEN_ASSIGN)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		ast_node_t *assigned_value_node;
		return expression(pdata, &assigned_value_node);
	}

	print_err_l("Expected a type declaration or an assignment operator after identifier", ERR_SYNTAX,
				pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Expression rule | <expression>
 *
 * @param pdata Parser data
 * @param return_node Address to which the address of the created AST note should be written
 * @return err_t Error code
 */
err_t expression(parser_data_t *pdata, ast_node_t **return_node)
{
	return parse_expression(pdata, NULL, return_node);
}

/**
 * @brief Expression rule with a seeded token at the beginning | <expression>
 *
 * @param pdata Parser data
 * @param return_node Address to which the address of the created AST note should be written
 * @param seed_token If specified, this token is used as the first token in the expression.
 * 					 If NULL, works the same as expression().
 * @return err_t Error code
 */
err_t expression_seeded(parser_data_t *pdata, ast_node_t **return_node, token_t *seed_token)
{
	return parse_expression(pdata, seed_token, return_node);
}

err_t parse(ast_node_t *program_node)
{
	parser_data_t pdata = {};

	err_t result = prologue(&pdata, program_node);

	return result;
}
