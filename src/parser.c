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
err_t ext_def(parser_data_t *pdata, ast_node_t *program_node);
err_t ext_def_list(parser_data_t *pdata, ast_node_t *program_node, unsigned list_indent);
err_t func_annotation(parser_data_t *pdata, ast_node_t *program_node);
err_t func_def(parser_data_t *pdata, ast_node_t *program_node);
err_t func_params(parser_data_t *pdata, symbol_data_t *table_entry);
err_t param_first(parser_data_t *pdata, symbol_data_t *table_entry);
err_t param_list(parser_data_t *pdata, symbol_data_t *table_entry);
err_t param(parser_data_t *pdata, param_t *param_out);
err_t return_type(parser_data_t *pdata, ast_node_t *func_call_node);
err_t block_start(parser_data_t *pdata, ast_node_t *block_node, unsigned header_indent_level);
err_t block(parser_data_t *pdata, ast_node_t *block_node, unsigned block_indent_level);
err_t type(parser_data_t *pdata, data_type_t *return_type);
err_t statement(parser_data_t *pdata, ast_node_t *block_node);
err_t expression_opt(parser_data_t *pdata, ast_node_t **return_value);
err_t else_opt(parser_data_t *pdata, unsigned header_indent_level);
err_t else_if_opt(parser_data_t *pdata, unsigned header_indent_level);
err_t for_generator(parser_data_t *pdata);
err_t for_by(parser_data_t *pdata);
err_t for_guard(parser_data_t *pdata);
err_t val_var_def(parser_data_t *pdata, ast_node_t *var_def_node, symbol_data_t *table_entry);
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
		result = ext_def(pdata, program_node);
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
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t ext_def(parser_data_t *pdata, ast_node_t *program_node)
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
	result = param(pdata, &pdata->last_extension_param);
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

	// These flags are used in order to create the correct node in <func-def>
	pdata->in_main = false;
	pdata->in_extension = true;

	// <func-def>
	result = func_def(pdata, program_node);
	if (result)
		return result;

	// <ext-def-list>
	return ext_def_list(pdata, program_node, extension_list_indent);
}

/**
 * @brief Rule for lists of extension function declarations | <ext-def-list>
 *
 * @param pdata Parser data
 * @param program_node AST program node
 * @param list_indent Indentation level of the first extension definition, which all the others have to match
 * @return err_t Error code
 */
err_t ext_def_list(parser_data_t *pdata, ast_node_t *program_node, unsigned list_indent)
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

		// These flags are used in order to create the correct node in <func-def>
		pdata->in_main = false;
		pdata->in_extension = true;

		// <func-def>
		result = func_def(pdata, program_node);
		if (result)
			return result;

		// <ext-def-list>
		return ext_def_list(pdata, program_node, list_indent);
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

		// These flags are used in order to create the correct node in <func-def>
		pdata->in_main = true;
		pdata->in_extension = false;

		return func_def(pdata, program_node);
	}
	// def, go to <func-def>
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_DEF)
	{
		if (pdata->token.indent != 0)
		{
			print_err_l("Function definition must be at indent level 0.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}

		// These flags are used in order to create the correct node in <func-def>
		pdata->in_main = false;
		pdata->in_extension = false;

		return func_def(pdata, program_node);
	}

	print_err_l("Expeced a function declaration.", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Rule for function declarations | <func-def>
 *
 * @param pdata Parser data
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t func_def(parser_data_t *pdata, ast_node_t *program_node)
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

	ast_node_t *def_node;
	symbol_data_t *table_entry;
	// Create a new AST node and a symbol table entry
	if (pdata->in_extension)
	{
		// FIXME: Extension should probably be able to have the same name if they are extending a different type
		// If the extension was already defined, throw an error
		if (symtable_item_defined(&pdata->extension_table, pdata->token.attribute.string))
		{
			print_err_l("An extension can only be defined once.", ERR_SEMANTIC_REDEFINITION, pdata->token.line);
			return ERR_SEMANTIC_REDEFINITION;
		}

		// Create a symbol table entry
		table_entry = symtable_item_create(&pdata->extension_table, pdata->token.attribute.string);
		if (!table_entry)
		{
			print_err_l("Failed to create a symbol table entry in func_def().", ERR_INTERNAL, pdata->token.line);
			return ERR_INTERNAL;
		}
		table_entry->symbol_type = SYM_EXTENSION;
		table_entry->data.extension.is_defined = true;
		table_entry->data.extension.base_param = pdata->last_extension_param;

		// Create an AST node
		def_node = ast_ext_def_make(pdata->last_extension_param.type);
		if (!def_node)
		{
			print_err_l("Failed to create an AST Node in func_def().", ERR_INTERNAL, pdata->token.line);
			return ERR_INTERNAL;
		}
		result = ast_ext_def_set_id(def_node, pdata->token.attribute.string);
		if (result)
			return result;
		result = ast_program_add_ext_def(program_node, def_node);
		if (result)
			return result;
	}
	else
	{
		// If the function was already defined, throw an error
		if (symtable_item_defined(&pdata->function_table, pdata->token.attribute.string))
		{
			print_err_l("A function can only be defined once.", ERR_SEMANTIC_REDEFINITION, pdata->token.line);
			return ERR_SEMANTIC_REDEFINITION;
		}

		// Create a symbol table entry
		table_entry = symtable_item_create(&pdata->function_table, pdata->token.attribute.string);
		if (!table_entry)
		{
			print_err_l("Failed to create a symbol table entry in func_def().", ERR_INTERNAL, pdata->token.line);
			return ERR_INTERNAL;
		}
		table_entry->symbol_type = SYM_FUNCTION;
		table_entry->data.function.is_defined = true;
		table_entry->data.function.is_main = pdata->in_main;

		// Create an AST node
		def_node = ast_func_def_make();
		if (!def_node)
		{
			print_err_l("Failed to create an AST Node in func_def().", ERR_INTERNAL, pdata->token.line);
			return ERR_INTERNAL;
		}
		result = ast_func_def_set_id(def_node, pdata->token.attribute.string);
		if (result)
			return result;
		result = ast_program_add_func_def(program_node, def_node);
		if (result)
			return result;
	}

	result = get_next_token(&pdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	// Create a symbol table for the function
	pdata->local_var_table = local_table_create(NULL);
	if (!pdata->local_var_table)
	{
		print_err_l("Failed to create a local symbol table.", ERR_INTERNAL, pdata->token.line);
		return ERR_INTERNAL;
	}

	// TODO: Add parameters and extension base into the symbol table

	// <func-params>
	result = func_params(pdata, table_entry);
	if (result)
		return result;

	// <return-type>
	result = return_type(pdata, def_node);

	// Destroy the local var table;
	pdata->local_var_table = local_table_destroy(pdata->local_var_table);
	if (pdata->local_var_table)
	{
		print_err_l("A symbol table destroyed in func_def() still has a parent, this should not happen.", ERR_INTERNAL,
					pdata->token.line);
		return ERR_INTERNAL;
	}

	return result;
}

/**
 * @brief Rule for the start of function parameter declaration | <func-params>
 *
 * @param pdata Parser data
 * @param table_entry Symbol table entry for the function definition
 * @return err_t Error code
 */
err_t func_params(parser_data_t *pdata, symbol_data_t *table_entry)
{
	// (, go to <param-first>
	if (pdata->token.type == TOKEN_OPENING_BRACKET)
	{
		err_t result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		return param_first(pdata, table_entry);
	}

	// Epsilon rule
	return ERR_OK;
}

/**
 * @brief First parameter of a parameter list rule | <param-first>
 *
 * @param pdata Parser data
 * @param table_entry Symbol table entry for the function definition
 * @return err_t Error code
 */
err_t param_first(parser_data_t *pdata, symbol_data_t *table_entry)
{
	err_t result;

	// Create a new empty parameter list
	result = symbol_add_param_list(table_entry);
	if (result)
		return result;

	// ), go to <func-params>
	if (pdata->token.type == TOKEN_CLOSING_BRACKET)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		return func_params(pdata, table_entry);
	}
	// Identifier
	else if (pdata->token.type == TOKEN_IDENTIFIER)
	{
		// Create new empty parameter
		result = symbol_add_param(table_entry);
		if (result)
			return result;

		// <param>
		result = param(pdata, symbol_top_param(table_entry));
		if (result)
			return result;

		// <param-list>
		return param_list(pdata, table_entry);
	}

	print_err_l("Expected a parameter or an end of the parameter list.", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Continuation of a parameter list | <param-list>
 *
 * @param pdata Parser data
 * @param table_entry Symbol table entry for the function definition
 * @return err_t Error code
 */
err_t param_list(parser_data_t *pdata, symbol_data_t *table_entry)
{
	err_t result;

	// ), go to <func-params>
	if (pdata->token.type == TOKEN_CLOSING_BRACKET)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		return func_params(pdata, table_entry);
	}
	// ","
	else if (pdata->token.type == TOKEN_COMMA)
	{
		// Create a new empty parameter
		result = symbol_add_param(table_entry);
		if (result)
			return result;

		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		result = param(pdata, symbol_top_param(table_entry));
		if (result)
			return result;

		return param_list(pdata, table_entry);
	}

	print_err_l("Expected a comma or and end of a parameter list.", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Function parameter definition rule | <param>
 *
 * @param pdata Parser data
 * @param param_out Pointer to which the resulting parameter struct will be written
 * @return err_t Error code
 */
err_t param(parser_data_t *pdata, param_t *param_out)
{
	err_t result;

	// Identifier
	if (pdata->token.type != TOKEN_IDENTIFIER)
	{
		print_err_l("Expected an identifier.", ERR_SYNTAX, pdata->token.line);
		return ERR_SYNTAX;
	}

	// Set the identifier of the parameter
	param_out->identifier = malloc(strlen(pdata->token.attribute.string) + 1);
	if (!param_out->identifier)
	{
		print_err_l("Failed to allocate memory in param().", ERR_INTERNAL, pdata->token.line);
		return ERR_INTERNAL;
	}
	strcpy(param_out->identifier, pdata->token.attribute.string);

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
	return type(pdata, &param_out->type);
}

/**
 * @brief Function return type definition | <return-type>
 *
 * @param pdata Parser data
 * @param func_call_node AST function call node
 * @return err_t Error code
 */
err_t return_type(parser_data_t *pdata, ast_node_t *func_call_node)
{
	err_t result;

	unsigned header_indentation = pdata->token.indent;

	symbol_data_t *entry;
	if (pdata->in_extension)
		entry = symtable_item_find(&pdata->extension_table, func_call_node->ext_def.unique_id);
	else
		entry = symtable_item_find(&pdata->function_table, func_call_node->func_def.unique_id);
	if (!entry)
	{
		print_err_l("Failed to find a symbol that was previously created in a symbol table. This should not happen.",
					ERR_INTERNAL, pdata->token.line);
		return ERR_INTERNAL;
	}

	// Create the block node which will be filled with statements and put it into the function def node
	ast_node_t *block_node = ast_block_make();
	if (!block_node)
	{
		print_err_l("Failed to create an AST node in return_type().", ERR_INTERNAL, pdata->token.line);
		return ERR_INTERNAL;
	}

	if (pdata->in_extension)
		result = ast_ext_def_set_block(func_call_node, block_node);
	else
		result = ast_func_def_set_block(func_call_node, block_node);
	if (result)
		return result;

	// =, go to <block-start>
	if (pdata->token.type == TOKEN_ASSIGN)
	{
		// Set the function return type to none, will be set at the end of the block
		if (pdata->in_extension)
			entry->data.extension.return_type = TYPE_NONE;
		else
			entry->data.function.return_type = TYPE_NONE;

		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		return block_start(pdata, block_node, header_indentation);
	}
	// :
	else if (pdata->token.type == TOKEN_COLON)
	{
		// <type>
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// Set the return type
		if (pdata->in_extension)
			result = type(pdata, &entry->data.extension.return_type);
		else
			result = type(pdata, &entry->data.function.return_type);
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
		return block_start(pdata, block_node, header_indentation);
	}

	print_err_l("Expected a return type definiton, or a block start delimiter (=).", ERR_SYNTAX, pdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Statement block start | <block-start>
 *
 * @param pdata Parser data
 * @param block_node AST block node
 * @param header_indent_level Indentation level of the block header
 * @return err_t Error code
 */
err_t block_start(parser_data_t *pdata, ast_node_t *block_node, unsigned header_indent_level)
{
	bool is_inline = !pdata->token.starts_logical_line;

	// If the first statement is on the same line as the header, there is only one statement allowed
	if (is_inline)
	{
		return statement(pdata, block_node);
	}

	unsigned statement_indent_level = pdata->token.indent;
	if (pdata->token.starts_logical_line && statement_indent_level <= header_indent_level)
	{
		print_err_l("Start of a block must be on a higher indentation level than the header.", ERR_SYNTAX,
					pdata->token.line);
		return ERR_SYNTAX;
	}

	// <statement>
	err_t result = statement(pdata, block_node);
	if (result)
		return result;

	// <block>
	return block(pdata, block_node, statement_indent_level);
}

/**
 * @brief Statement block | <block>
 *
 * @param pdata Parser data
 * @param block_node AST block node
 * @param block_indent_level Indentation level every statement in the block must match
 * @return err_t Error code
 */
err_t block(parser_data_t *pdata, ast_node_t *block_node, unsigned block_indent_level)
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

	err_t result = statement(pdata, block_node);
	if (result)
		return result;

	return block(pdata, block_node, block_indent_level);
}

/**
 * @brief Type parsing rule | <type>
 *
 * @param pdata Parser data
 * @param return_type Pointer to where the returned type should be written
 * @return err_t Error code
 */
err_t type(parser_data_t *pdata, data_type_t *return_type)
{
	bool valid_keyword = false;

	if (pdata->token.type == TOKEN_KEYWORD)
	{
		switch (pdata->token.attribute.keyword)
		{
		case KW_INT:
			valid_keyword = true;
			*return_type = TYPE_INT;
			break;
		case KW_DOUBLE:
			valid_keyword = true;
			*return_type = TYPE_DOUBLE;
			break;
		case KW_STRING:
			valid_keyword = true;
			*return_type = TYPE_STRING;
			break;
		case KW_UNIT:
			valid_keyword = true;
			*return_type = TYPE_UNIT;
			break;
		case KW_BOOLEAN:
			valid_keyword = true;
			*return_type = TYPE_BOOLEAN;
			break;
		default:
			*return_type = TYPE_NONE;
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
 * @param block_node AST block node
 * @return err_t Error code
 */
err_t statement(parser_data_t *pdata, ast_node_t *block_node)
{
	err_t result;
	unsigned statement_indent_level = pdata->token.indent;

	// Create the node for the statement and add it to the block
	// The node will be filled based on the rule
	ast_node_t *statement_node;

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

		// Check if the variable has already been defined
		if (symtable_item_defined(pdata->local_var_table, pdata->token.attribute.string))
		{
			print_err_l("Variable name has already been defined!", ERR_SEMANTIC_REDEFINITION, pdata->token.line);
			return ERR_SEMANTIC_REDEFINITION;
		}

		// Create a symbol table entry
		symbol_data_t *entry = symtable_item_create(pdata->local_var_table, pdata->token.attribute.string);
		if (!entry)
		{
			print_err_l("Failed to create a symbol table entry in statement().", ERR_INTERNAL, pdata->token.line);
			return ERR_INTERNAL;
		}
		entry->symbol_type = SYM_VARIABLE;
		entry->data.variable.mutable = false;
		// entry->data.variable.unique_name // FIXME: Generate a unique name for the variable

		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// Create an AST node
		statement_node = ast_var_def_make(entry->data.variable.unique_name);

		// <val-var-def>
		result = val_var_def(pdata, statement_node, entry);
		if (result)
			return result;

		return ast_block_add_statement(block_node, statement_node);
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

		// Check if the variable has already been defined
		if (symtable_item_defined(pdata->local_var_table, pdata->token.attribute.string))
		{
			print_err_l("Variable name has already been defined!", ERR_SEMANTIC_REDEFINITION, pdata->token.line);
			return ERR_SEMANTIC_REDEFINITION;
		}

		// Create a symbol table entry
		symbol_data_t *entry = symtable_item_create(pdata->local_var_table, pdata->token.attribute.string);
		if (!entry)
		{
			print_err_l("Failed to create a symbol table entry in statement().", ERR_INTERNAL, pdata->token.line);
			return ERR_INTERNAL;
		}
		entry->symbol_type = SYM_VARIABLE;
		entry->data.variable.mutable = true;
		// entry->data.variable.unique_name // FIXME: Generate a unique name for the variable

		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// Create an AST node
		statement_node = ast_var_def_make(entry->data.variable.unique_name);

		// <val-var-def>
		result = val_var_def(pdata, statement_node, entry);
		if (result)
			return result;

		return ast_block_add_statement(block_node, statement_node);
	}
	// if
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_IF)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *condition_node; // FIXME:
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
		ast_node_t *inner_block_node = NULL; // FIXME:
		result = block_start(pdata, inner_block_node, statement_indent_level);
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
		ast_node_t *condition_node; // FIXME:
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
		ast_node_t *inner_block_node = NULL; // FIXME:
		return block_start(pdata, inner_block_node, statement_indent_level);
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
		ast_node_t *iterator_node; // FIXME:
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
		ast_node_t *inner_block_node = NULL; // FIXME:
		return block_start(pdata, inner_block_node, statement_indent_level);
	}
	// return
	else if (pdata->token.type == TOKEN_KEYWORD && pdata->token.attribute.keyword == KW_RETURN)
	{
		result = get_next_token(&pdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <expression>
		ast_node_t *return_value_node;
		result = expression_opt(pdata, &return_value_node);
		if (result)
			return result;

		statement_node = ast_return_call_make(return_value_node);
		if (!statement_node)
		{
			print_err_l("Failed to create an AST node in statement().", ERR_INTERNAL, pdata->token.line);
			return ERR_INTERNAL;
		}
		return ast_block_add_statement(block_node, statement_node);
	}
	// identifier
	else if (pdata->token.type == TOKEN_IDENTIFIER)
	{
		// A lookahead needs to be performed here in order to
		// tell between the rules:
		// id = <expression>
		// <expression>

		// FIXME: The string most probably needs to be copied here
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

			// Create node with the assigned value, wrap it in an assignment node, then add it to the block
			ast_node_t *assigned_expression_node;

			result = expression(pdata, &assigned_expression_node);
			if (result)
				return result;

			statement_node = ast_var_assign_make(id_token.attribute.string, assigned_expression_node);
			if (!statement_node)
			{
				print_err_l("Failed to create an AST node in statement().", ERR_INTERNAL, pdata->token.line);
				return ERR_INTERNAL;
			}
			return ast_block_add_statement(block_node, statement_node);
		}

		// <expression>
		// The expression parser needs to have the id token seeded before the token in pdata
		result = expression_seeded(pdata, &statement_node, &id_token);
		if (result)
			return result;
		return ast_block_add_statement(block_node, statement_node);
	}

	// <expression>
	result = expression(pdata, &statement_node);
	if (result)
		return result;

	return ast_block_add_statement(block_node, statement_node);
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

		*return_node = ast_literal_make(&lit);
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
		ast_node_t *condition_node; // FIXME:
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
		ast_node_t *inner_block_node = NULL; // FIXME:
		result = block_start(pdata, inner_block_node, header_indent_level);
		if (result)
			return result;

		// <else-opt>
		return else_opt(pdata, header_indent_level);
	}

	// <block_start>
	ast_node_t *inner_block_node = NULL; // FIXME:
	return block_start(pdata, inner_block_node, header_indent_level);
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
			ast_node_t *to_node; // FIXME:
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
			ast_node_t *until_node; // FIXME:
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
		ast_node_t *by_node; // FIXME:
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
		ast_node_t *condition_node; // FIXME:
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
 * @param var_def_node AST variable definition node
 * @param table_entry Entry in the lcoal symbol table of
 * @return err_t Error code
 */
err_t val_var_def(parser_data_t *pdata, ast_node_t *var_def_node, symbol_data_t *table_entry)
{
	err_t result;

	// :
	if (pdata->token.type == TOKEN_COLON)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <type>
		result = type(pdata, &table_entry->data.variable.type);
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

		// <expression>
		ast_node_t *assigned_value_node;
		result = expression(pdata, &assigned_value_node);
		if (result)
			return result;

		// If types dont match, throw a semantic error
		if (table_entry->data.variable.type != assigned_value_node->eval_type)
		{
			print_err_l("Type of the expression assigned to variable does not match its type.",
						ERR_SEMANTIC_INVALID_TYPE, pdata->token.line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		return ast_var_def_set_assignment(var_def_node, assigned_value_node);
	}

	// =
	if (pdata->token.type == TOKEN_ASSIGN)
	{
		result = get_next_token(&pdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *assigned_value_node;
		result = expression(pdata, &assigned_value_node);
		if (result)
			return result;

		// Set the type of the variable
		table_entry->data.variable.type = assigned_value_node->eval_type;

		return ast_var_def_set_assignment(var_def_node, assigned_value_node);
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
