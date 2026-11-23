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
err_t prologue(compiler_data_t *cdata, ast_node_t *program_node);
err_t func_list(compiler_data_t *cdata, ast_node_t *program_node);
err_t ext_def(compiler_data_t *cdata, ast_node_t *program_node);
err_t ext_def_list(compiler_data_t *cdata, ast_node_t *program_node, unsigned list_indent);
err_t func_annotation(compiler_data_t *cdata, ast_node_t *program_node);
err_t func_def(compiler_data_t *cdata, ast_node_t *program_node);
err_t func_params(compiler_data_t *cdata, symbol_data_t *table_entry);
err_t param_first(compiler_data_t *cdata, symbol_data_t *table_entry);
err_t param_list(compiler_data_t *cdata, symbol_data_t *table_entry);
err_t param(compiler_data_t *cdata, param_t *param_out);
err_t return_type(compiler_data_t *cdata, ast_node_t *func_call_node);
err_t block_start(compiler_data_t *cdata, ast_node_t *block_node, unsigned header_indent_level);
err_t block(compiler_data_t *cdata, ast_node_t *block_node, unsigned block_indent_level);
err_t type(compiler_data_t *cdata, data_type_t *return_type);
err_t statement(compiler_data_t *cdata, ast_node_t *block_node);
err_t expression_opt(compiler_data_t *cdata, ast_node_t **return_value);
err_t else_opt(compiler_data_t *cdata, ast_node_t **out_node, unsigned header_indent_level);
err_t else_if_opt(compiler_data_t *cdata, ast_node_t **out_node, unsigned header_indent_level);
err_t for_generator(compiler_data_t *cdata, ast_node_t *for_node);
err_t for_by(compiler_data_t *cdata, ast_node_t *for_node);
err_t for_guard(compiler_data_t *cdata, ast_node_t *for_node);
err_t val_var_def(compiler_data_t *cdata, ast_node_t *var_def_node, symbol_data_t *table_entry);
err_t expression(compiler_data_t *cdata, ast_node_t **return_node);

/**
 * @brief The entry rule of syntax analysis | <prologue>
 *
 * @param cdata Compiler data
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t prologue(compiler_data_t *cdata, ast_node_t *program_node)
{
	// Keyword import
	err_t result = get_next_token(&cdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	if (cdata->token.indent != 0)
	{
		print_err_l("The program prologue must be at indentation level 0.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	if (cdata->token.type != TOKEN_KEYWORD || cdata->token.attribute.keyword != KW_IMPORT)
	{
		print_err_l("Invalid token. Expected the \"import\" keyword.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	// Identifier ifj26
	result = get_next_token(&cdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	if (cdata->token.type != TOKEN_IDENTIFIER)
	{
		print_err_l("Invalid token. Expected an identifier.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}
	if (strcmp(cdata->token.attribute.string, "ifj26"))
	{
		print_err_l("Invalid token. Expected the \"ifj26\" identifier.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	// Dot
	result = get_next_token(&cdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	if (cdata->token.type != TOKEN_DOT)
	{
		print_err_l("Invalid token. Expected a \".\".", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	// Asterisk
	result = get_next_token(&cdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	if (cdata->token.type != TOKEN_ASTERISK)
	{
		print_err_l("Invalid token. Expected a \"*\".", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	// <func-list>
	result = get_next_token(&cdata->token, EOL_REQUIRED);
	if (result)
		return result;

	return func_list(cdata, program_node);
}

/**
 * @brief List of global functions or extension definitons | <func-list>
 *
 * @param cdata Compiler data
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t func_list(compiler_data_t *cdata, ast_node_t *program_node)
{
	err_t result;

	// @main or def, go to <func-annotation>
	if (cdata->token.type == TOKEN_KEYWORD &&
		(cdata->token.attribute.keyword == KW_MAIN || cdata->token.attribute.keyword == KW_DEF))
	{
		result = func_annotation(cdata, program_node);
		if (result)
			return result;

		return func_list(cdata, program_node);
	}
	// extension
	else if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_EXTENSION)
	{
		// <ext-def>
		result = ext_def(cdata, program_node);
		if (result)
			return result;

		// <func-list>
		return func_list(cdata, program_node);
	}
	// If EOF is received, end the function list
	else if (cdata->token.type == TOKEN_EOF)
	{
		return ERR_OK;
	}

	print_err_l("Expected a function or extension declaration.", ERR_SYNTAX, cdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Rule for extension definitions | <ext-def>
 *
 * @param cdata Compiler data
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t ext_def(compiler_data_t *cdata, ast_node_t *program_node)
{
	err_t result;

	// extension
	if (cdata->token.type != TOKEN_KEYWORD || cdata->token.attribute.keyword != KW_EXTENSION)
	{
		print_err_l("Expected a type extension block declaration", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	if (cdata->token.indent != 0)
	{
		print_err_l("Extension block header must be at indent level 0.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	result = get_next_token(&cdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	// (
	if (cdata->token.type != TOKEN_OPENING_BRACKET)
	{
		print_err_l("Missing \"(\" after an extension keyword.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&cdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	// <param>
	result = param(cdata, &cdata->last_extension_param);
	if (result)
		return result;

	// )
	if (cdata->token.type != TOKEN_CLOSING_BRACKET)
	{
		print_err_l("Missing \")\" after an extension type declaration.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&cdata->token, EOL_REQUIRED);
	if (result)
		return result;

	unsigned extension_list_indent = cdata->token.indent;
	if (extension_list_indent == 0)
	{
		print_err_l("Extensions methods must be indented.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	// These flags are used in order to create the correct node in <func-def>
	cdata->in_main = false;
	cdata->in_extension = true;

	// <func-def>
	result = func_def(cdata, program_node);
	if (result)
		return result;

	// <ext-def-list>
	return ext_def_list(cdata, program_node, extension_list_indent);
}

/**
 * @brief Rule for lists of extension function declarations | <ext-def-list>
 *
 * @param cdata Compiler data
 * @param program_node AST program node
 * @param list_indent Indentation level of the first extension definition, which all the others have to match
 * @return err_t Error code
 */
err_t ext_def_list(compiler_data_t *cdata, ast_node_t *program_node, unsigned list_indent)
{
	if (cdata->token.indent > list_indent)
	{
		print_err_l("All extension method definitions must be at the same indent level.", ERR_SYNTAX,
					cdata->token.line);
		return ERR_SYNTAX;
	}
	else if (cdata->token.indent < list_indent)
		return ERR_OK;

	// def
	if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_DEF)
	{
		err_t result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// These flags are used in order to create the correct node in <func-def>
		cdata->in_main = false;
		cdata->in_extension = true;

		// <func-def>
		result = func_def(cdata, program_node);
		if (result)
			return result;

		// <ext-def-list>
		return ext_def_list(cdata, program_node, list_indent);
	}

	// Epsilon rule
	return ERR_OK;
}

/**
 * @brief Rule for the main function annotiation | <func-annotation>
 *
 * @param cdata Compiler data
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t func_annotation(compiler_data_t *cdata, ast_node_t *program_node)
{
	// @main, go to <func-def>
	if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_MAIN)
	{
		if (cdata->token.indent != 0)
		{
			print_err_l("Function annotation must be at indent level 0.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}

		err_t result;

		result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		if (cdata->main_defined)
		{
			print_err_l("Only one function can be annotated as @main.", ERR_SEMANTIC_REDEFINITION, cdata->token.line);
			return ERR_SEMANTIC_REDEFINITION;
		}

		// These flags are used in order to create the correct node in <func-def>
		cdata->in_main = true;
		cdata->main_defined = true;
		cdata->in_extension = false;

		return func_def(cdata, program_node);
	}
	// def, go to <func-def>
	else if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_DEF)
	{
		if (cdata->token.indent != 0)
		{
			print_err_l("Function definition must be at indent level 0.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}

		// These flags are used in order to create the correct node in <func-def>
		cdata->in_main = false;
		cdata->in_extension = false;

		return func_def(cdata, program_node);
	}

	print_err_l("Expeced a function declaration.", ERR_SYNTAX, cdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Rule for function declarations | <func-def>
 *
 * @param cdata Compiler data
 * @param program_node AST program node
 * @return err_t Error code
 */
err_t func_def(compiler_data_t *cdata, ast_node_t *program_node)
{
	// def
	if (cdata->token.type != TOKEN_KEYWORD || cdata->token.attribute.keyword != KW_DEF)
	{
		print_err_l("Expected a function declaration.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	// Function identifier
	err_t result = get_next_token(&cdata->token, EOL_FORBIDDEN);
	if (result)
		return result;
	if (cdata->token.type != TOKEN_IDENTIFIER)
	{
		print_err_l("Expected an identifier in a function declaration.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	ast_node_t *def_node;
	symbol_data_t *table_entry;
	// Create a new AST node and a symbol table entry
	if (cdata->in_extension)
	{
		// FIXME: Extension should probably be able to have the same name if they are extending a different type
		// If the extension was already defined, throw an error
		if (symtable_item_defined(&cdata->extension_table, cdata->token.attribute.string))
		{
			print_err_l("An extension can only be defined once.", ERR_SEMANTIC_REDEFINITION, cdata->token.line);
			return ERR_SEMANTIC_REDEFINITION;
		}

		// Create a symbol table entry
		table_entry = symtable_item_create(&cdata->extension_table, cdata->token.attribute.string);
		if (!table_entry)
		{
			print_err_l("Failed to create a symbol table entry in func_def().", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}
		table_entry->symbol_type = SYM_EXTENSION;
		table_entry->data.extension.is_defined = true;
		table_entry->data.extension.base_param = cdata->last_extension_param;

		// Create an AST node
		def_node = ast_ext_def_make(cdata->last_extension_param.type);
		if (!def_node)
		{
			print_err_l("Failed to create an AST Node in func_def().", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}
		result = ast_ext_def_set_id(def_node, cdata->token.attribute.string);
		if (result)
			return result;
		result = ast_program_add_ext_def(program_node, def_node);
		if (result)
			return result;
	}
	else
	{
		// If the function was already defined, throw an error
		if (symtable_item_defined(&cdata->function_table, cdata->token.attribute.string))
		{
			print_err_l("A function can only be defined once.", ERR_SEMANTIC_REDEFINITION, cdata->token.line);
			return ERR_SEMANTIC_REDEFINITION;
		}

		// Create a symbol table entry
		table_entry = symtable_item_create(&cdata->function_table, cdata->token.attribute.string);
		if (!table_entry)
		{
			print_err_l("Failed to create a symbol table entry in func_def().", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}
		table_entry->symbol_type = SYM_FUNCTION;
		table_entry->data.function.is_defined = true;
		table_entry->data.function.is_main = cdata->in_main;

		// Create an AST node
		def_node = ast_func_def_make();
		if (!def_node)
		{
			print_err_l("Failed to create an AST Node in func_def().", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}
		result = ast_func_def_set_id(def_node, cdata->token.attribute.string);
		if (result)
			return result;
		result = ast_program_add_func_def(program_node, def_node);
		if (result)
			return result;
	}

	result = get_next_token(&cdata->token, EOL_FORBIDDEN);
	if (result)
		return result;

	// Create a symbol table for the function
	cdata->local_var_table = local_table_create(&cdata->function_table);
	if (!cdata->local_var_table)
	{
		print_err_l("Failed to create a local symbol table.", ERR_INTERNAL, cdata->token.line);
		return ERR_INTERNAL;
	}

	// <func-params>
	result = func_params(cdata, table_entry);
	if (result)
		return result;

	// Add parameters and extension base into the symbol table
	result = symbol_add_parameters_into_table(table_entry, cdata->local_var_table);
	if (result)
		return result;

	// <return-type>
	result = return_type(cdata, def_node);

	// Destroy the local var table;
	cdata->local_var_table = local_table_destroy(cdata->local_var_table);
	if (cdata->local_var_table->parent)
	{
		print_err_l("A symbol table destroyed in func_def() does not have the function table as a parent, this should "
					"not happen.",
					ERR_INTERNAL, cdata->token.line);
		return ERR_INTERNAL;
	}

	return result;
}

/**
 * @brief Rule for the start of function parameter declaration | <func-params>
 *
 * @param cdata Compiler data
 * @param table_entry Symbol table entry for the function definition
 * @return err_t Error code
 */
err_t func_params(compiler_data_t *cdata, symbol_data_t *table_entry)
{
	// (, go to <param-first>
	if (cdata->token.type == TOKEN_OPENING_BRACKET)
	{
		err_t result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		return param_first(cdata, table_entry);
	}

	// Epsilon rule
	return ERR_OK;
}

/**
 * @brief First parameter of a parameter list rule | <param-first>
 *
 * @param cdata Compiler data
 * @param table_entry Symbol table entry for the function definition
 * @return err_t Error code
 */
err_t param_first(compiler_data_t *cdata, symbol_data_t *table_entry)
{
	err_t result;

	// Create a new empty parameter list
	result = symbol_add_param_list(table_entry);
	if (result)
		return result;

	// ), go to <func-params>
	if (cdata->token.type == TOKEN_CLOSING_BRACKET)
	{
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		return func_params(cdata, table_entry);
	}
	// Identifier
	else if (cdata->token.type == TOKEN_IDENTIFIER)
	{
		// Create new empty parameter
		result = symbol_add_param(table_entry);
		if (result)
			return result;

		// <param>
		result = param(cdata, symbol_top_param(table_entry));
		if (result)
			return result;

		// <param-list>
		return param_list(cdata, table_entry);
	}

	print_err_l("Expected a parameter or an end of the parameter list.", ERR_SYNTAX, cdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Continuation of a parameter list | <param-list>
 *
 * @param cdata Compiler data
 * @param table_entry Symbol table entry for the function definition
 * @return err_t Error code
 */
err_t param_list(compiler_data_t *cdata, symbol_data_t *table_entry)
{
	err_t result;

	// ), go to <func-params>
	if (cdata->token.type == TOKEN_CLOSING_BRACKET)
	{
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		return func_params(cdata, table_entry);
	}
	// ","
	else if (cdata->token.type == TOKEN_COMMA)
	{
		// Create a new empty parameter
		result = symbol_add_param(table_entry);
		if (result)
			return result;

		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		result = param(cdata, symbol_top_param(table_entry));
		if (result)
			return result;

		return param_list(cdata, table_entry);
	}

	print_err_l("Expected a comma or and end of a parameter list.", ERR_SYNTAX, cdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Function parameter definition rule | <param>
 *
 * @param cdata Compiler data
 * @param param_out Pointer to which the resulting parameter struct will be written
 * @return err_t Error code
 */
err_t param(compiler_data_t *cdata, param_t *param_out)
{
	err_t result;

	// Identifier
	if (cdata->token.type != TOKEN_IDENTIFIER)
	{
		print_err_l("Expected an identifier.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}

	// Set the identifier of the parameter
	param_out->identifier = malloc(strlen(cdata->token.attribute.string) + 1);
	if (!param_out->identifier)
	{
		print_err_l("Failed to allocate memory in param().", ERR_INTERNAL, cdata->token.line);
		return ERR_INTERNAL;
	}
	strcpy(param_out->identifier, cdata->token.attribute.string);

	result = get_next_token(&cdata->token, EOL_OPTIONAL);
	if (result)
		return result;
	// :
	if (cdata->token.type != TOKEN_COLON)
	{
		print_err_l("Expected a \":\" separator between identifier and type.", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}
	result = get_next_token(&cdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	// Type
	return type(cdata, &param_out->type);
}

/**
 * @brief Function return type definition | <return-type>
 *
 * @param cdata Compiler data
 * @param func_call_node AST function call node
 * @return err_t Error code
 */
err_t return_type(compiler_data_t *cdata, ast_node_t *func_call_node)
{
	err_t result;

	unsigned header_indentation = cdata->token.indent;

	symbol_data_t *entry;
	if (cdata->in_extension)
		entry = symtable_item_find(&cdata->extension_table, func_call_node->ext_def.unique_id);
	else
		entry = symtable_item_find(&cdata->function_table, func_call_node->func_def.unique_id);
	if (!entry)
	{
		print_err_l("Failed to find a symbol that was previously created in a symbol table. This should not happen.",
					ERR_INTERNAL, cdata->token.line);
		return ERR_INTERNAL;
	}

	// Create the block node which will be filled with statements and put it into the function def node
	ast_node_t *block_node = ast_block_make();
	if (!block_node)
	{
		print_err_l("Failed to create an AST node in return_type().", ERR_INTERNAL, cdata->token.line);
		return ERR_INTERNAL;
	}

	if (cdata->in_extension)
		result = ast_ext_def_set_block(func_call_node, block_node);
	else
		result = ast_func_def_set_block(func_call_node, block_node);
	if (result)
		return result;

	// =, go to <block-start>
	if (cdata->token.type == TOKEN_ASSIGN)
	{
		// Set the function return type to none, will be set at the end of the block
		if (cdata->in_extension)
			entry->data.extension.return_type = TYPE_NONE;
		else
			entry->data.function.return_type = TYPE_NONE;

		result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		return block_start(cdata, block_node, header_indentation);
	}
	// :
	else if (cdata->token.type == TOKEN_COLON)
	{
		// <type>
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// Set the return type
		if (cdata->in_extension)
			result = type(cdata, &entry->data.extension.return_type);
		else
			result = type(cdata, &entry->data.function.return_type);
		if (result)
			return result;

		// =
		if (cdata->token.type != TOKEN_ASSIGN)
		{
			print_err_l("Expected a block start delimiter (=).", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <block-start>
		return block_start(cdata, block_node, header_indentation);
	}

	print_err_l("Expected a return type definiton, or a block start delimiter (=).", ERR_SYNTAX, cdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Statement block start | <block-start>
 *
 * @param cdata Compiler data
 * @param block_node AST block node
 * @param header_indent_level Indentation level of the block header
 * @return err_t Error code
 */
err_t block_start(compiler_data_t *cdata, ast_node_t *block_node, unsigned header_indent_level)
{
	bool is_inline = !cdata->token.starts_logical_line;

	// If the first statement is on the same line as the header, there is only one statement allowed
	if (is_inline)
	{
		return statement(cdata, block_node);
	}

	unsigned statement_indent_level = cdata->token.indent;
	if (cdata->token.starts_logical_line && statement_indent_level <= header_indent_level)
	{
		print_err_l("Start of a block must be on a higher indentation level than the header.", ERR_SYNTAX,
					cdata->token.line);
		return ERR_SYNTAX;
	}

	// <statement>
	err_t result = statement(cdata, block_node);
	if (result)
		return result;

	// <block>
	return block(cdata, block_node, statement_indent_level);
}

/**
 * @brief Statement block | <block>
 *
 * @param cdata Compiler data
 * @param block_node AST block node
 * @param block_indent_level Indentation level every statement in the block must match
 * @return err_t Error code
 */
err_t block(compiler_data_t *cdata, ast_node_t *block_node, unsigned block_indent_level)
{
	while (cdata->token.type == TOKEN_SEMICOLON)
	{
		err_t result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;
	}

	if (cdata->token.type == TOKEN_EOF)
		return ERR_OK;

	unsigned statement_indent_level = cdata->token.indent;
	// Invalid
	if (statement_indent_level > block_indent_level)
	{
		print_err_l("All statements within a block must be on the same indentation level.", ERR_SYNTAX,
					cdata->token.line);
		return ERR_SYNTAX;
	}
	// End of the block
	else if (statement_indent_level < block_indent_level)
		return ERR_OK;

	err_t result = statement(cdata, block_node);
	if (result)
		return result;

	return block(cdata, block_node, block_indent_level);
}

/**
 * @brief Type parsing rule | <type>
 *
 * @param cdata Compiler data
 * @param return_type Pointer to where the returned type should be written
 * @return err_t Error code
 */
err_t type(compiler_data_t *cdata, data_type_t *return_type)
{
	bool valid_keyword = false;

	if (cdata->token.type == TOKEN_KEYWORD)
	{
		switch (cdata->token.attribute.keyword)
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
		// The fix is probably to hold a "in_parenthesis" value in the compiler data and decide based on that.
		return get_next_token(&cdata->token, EOL_OPTIONAL);
	}
	else
	{
		print_err_l("Expected a valid type (Int, Double, String, Unit, Boolean).", ERR_SYNTAX, cdata->token.line);
		return ERR_SYNTAX;
	}
}

/**
 * @brief Statements | <statement>
 *
 * @param cdata Compiler data
 * @param block_node AST block node
 * @return err_t Error code
 */
err_t statement(compiler_data_t *cdata, ast_node_t *block_node)
{
	err_t result;
	unsigned statement_indent_level = cdata->token.indent;

	// Create the node for the statement and add it to the block
	// The node will be filled based on the rule
	ast_node_t *statement_node;

	// val
	if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_VAL)
	{
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// id
		if (cdata->token.type != TOKEN_IDENTIFIER)
		{
			print_err_l("Expected an identifier in constant declaration.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}

		// Check if the variable has already been defined
		// FIXME: only the local symbol table has to be checked
		if (symtable_item_defined(cdata->local_var_table, cdata->token.attribute.string))
		{
			print_err_l("Variable name has already been defined!", ERR_SEMANTIC_REDEFINITION, cdata->token.line);
			return ERR_SEMANTIC_REDEFINITION;
		}

		// Create a symbol table entry
		symbol_data_t *entry = symtable_item_create(cdata->local_var_table, cdata->token.attribute.string);
		if (!entry)
		{
			print_err_l("Failed to create a symbol table entry in statement().", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}
		entry->symbol_type = SYM_VARIABLE;
		entry->data.variable.mutable = false;
		result = symbol_generate_unique_id(cdata->token.attribute.string, entry, cdata->local_var_table);
		if (result)
			return result;

		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// Create an AST node
		statement_node = ast_var_def_make(entry->data.variable.unique_name);

		// <val-var-def>
		result = val_var_def(cdata, statement_node, entry);
		if (result)
			return result;

		entry->data.variable.is_defined = true;
		return ast_block_add_statement(block_node, statement_node);
	}
	// var
	else if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_VAR)
	{
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// id
		if (cdata->token.type != TOKEN_IDENTIFIER)
		{
			print_err_l("Expected an identifier in variable declaration.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}

		// Check if the variable has already been defined
		if (symtable_item_defined(cdata->local_var_table, cdata->token.attribute.string))
		{
			print_err_l("Variable name has already been defined!", ERR_SEMANTIC_REDEFINITION, cdata->token.line);
			return ERR_SEMANTIC_REDEFINITION;
		}

		// Create a symbol table entry
		symbol_data_t *entry = symtable_item_create(cdata->local_var_table, cdata->token.attribute.string);
		if (!entry)
		{
			print_err_l("Failed to create a symbol table entry in statement().", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}
		entry->symbol_type = SYM_VARIABLE;
		entry->data.variable.mutable = true;
		result = symbol_generate_unique_id(cdata->token.attribute.string, entry, cdata->local_var_table);
		if (result)
			return result;

		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// Create an AST node
		statement_node = ast_var_def_make(entry->data.variable.unique_name);

		// <val-var-def>
		result = val_var_def(cdata, statement_node, entry);
		if (result)
			return result;

		entry->data.variable.is_defined = true;
		return ast_block_add_statement(block_node, statement_node);
	}
	// if
	else if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_IF)
	{
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *condition_node;
		result = expression(cdata, &condition_node);
		if (result)
			return result;

		// Check if the condition expression results in a boolean
		if (condition_node->eval_type != TYPE_BOOLEAN)
		{
			print_err_l("The condition of an if statement must result in a Boolean.", ERR_SEMANTIC_INVALID_TYPE,
						cdata->token.line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		// then
		if (cdata->token.type != TOKEN_KEYWORD || cdata->token.attribute.keyword != KW_THEN)
		{
			print_err_l("Expected a \"then\" keyword after expression.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// Create the AST node
		statement_node = ast_if_make(condition_node);
		if (!statement_node)
		{
			print_err_l("Failed to create an AST node for and if statement!", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}

		// <block-start>
		ast_node_t *then_block_node = ast_block_make();
		if (!then_block_node)
		{
			print_err_l("Failed to create a block node for an if statement!", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}

		result = block_start(cdata, then_block_node, statement_indent_level);
		if (result)
			return result;

		// Set the then branch
		result = ast_if_set_then_block(statement_node, then_block_node);
		if (result)
			return result;

		// Add the else branch. The node will be filled either with a block or an if node.
		// If the node remains NULL, there was no else branch
		ast_node_t *else_node = NULL;

		// <else-opt>
		result = else_opt(cdata, &else_node, statement_indent_level);
		if (result)
			return result;

		result = ast_if_set_else_node(statement_node, else_node);
		if (result)
			return result;

		return ast_block_add_statement(block_node, statement_node);
	}
	// while
	else if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_WHILE)
	{
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *condition_node; // FIXME:
		result = expression(cdata, &condition_node);
		if (result)
			return result;

		// do
		if (cdata->token.type != TOKEN_KEYWORD || cdata->token.attribute.keyword != KW_DO)
		{
			print_err_l("Expected a \"do\" keyword after expression.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&cdata->token, EOL_REQUIRED);
		if (result)
			return result;

		// <block-start>
		ast_node_t *inner_block_node = NULL; // FIXME:
		return block_start(cdata, inner_block_node, statement_indent_level);
	}
	// for
	else if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_FOR)
	{
		result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// identifier
		if (cdata->token.type != TOKEN_IDENTIFIER)
		{
			print_err_l("Expected an iterator identifier after \"for\".", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}

		// Copy the identifier for making the AST node later
		char *iterator_id = malloc(strlen(cdata->token.attribute.string) + 1);
		if (!iterator_id)
		{
			print_err_l("Failed to allocaty memory for the iterator identifier.", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}
		strcpy(iterator_id, cdata->token.attribute.string);

		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
		{
			free(iterator_id);
			return result;
		}

		// <-
		if (cdata->token.type != TOKEN_FOR_GEN_ASSIGN)
		{
			print_err_l("Expected a generator variable assignment operator (<-).", ERR_SYNTAX, cdata->token.line);
			free(iterator_id);
			return ERR_SYNTAX;
		}
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
		{
			free(iterator_id);
			return result;
		}

		// <expression>
		ast_node_t *from_node;
		result = expression(cdata, &from_node);
		if (result)
		{
			free(iterator_id);
			return result;
		}

		// First create the local variable table and add the iterator as a variable
		cdata->local_var_table = local_table_create(cdata->local_var_table);
		if (!cdata->local_var_table)
		{
			print_err("Failed to create a local variable table!", ERR_INTERNAL);
			free(iterator_id);
			return ERR_INTERNAL;
		}

		symbol_data_t *entry = symtable_item_create(cdata->local_var_table, iterator_id);
		if (!entry)
		{
			print_err("Failed to create a symbol table entry.", ERR_INTERNAL);
			free(iterator_id);
			return ERR_INTERNAL;
		}
		entry->symbol_type = SYM_VARIABLE;
		entry->data.variable.is_defined = true;
		entry->data.variable.mutable = true;
		entry->data.variable.type = TYPE_INT;
		symbol_generate_unique_id(iterator_id, entry, cdata->local_var_table);
		free(iterator_id);

		// Create the AST node for the for statement
		statement_node = ast_for_make(entry->data.variable.unique_name, from_node);
		if (!statement_node)
		{
			print_err_l("Failed to create an AST node for a for statement!", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}

		// <for-generator>
		result = for_generator(cdata, statement_node);
		if (result)
			return result;

		// <for-guard>
		result = for_guard(cdata, statement_node);
		if (result)
			return result;

		// do
		if (cdata->token.type != TOKEN_KEYWORD || cdata->token.attribute.keyword != KW_DO)
		{
			print_err_l("Expected the \"do\" keyword after a for guard.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&cdata->token, EOL_REQUIRED);
		if (result)
			return result;

		// <block-start>
		ast_node_t *inner_block_node = ast_block_make();
		if (!inner_block_node)
		{
			print_err_l("Failed to create a block node for a for statement!", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}
		result = block_start(cdata, inner_block_node, statement_indent_level);
		if (result)
			return result;

		result = ast_for_set_block(statement_node, inner_block_node);
		if (result)
			return result;

		// Destroy the local table
		cdata->local_var_table = local_table_destroy(cdata->local_var_table);
		if (!cdata->local_var_table)
		{
			print_err("Failed to destroy a local variable table!", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		return ast_block_add_statement(block_node, statement_node);
	}
	// return
	else if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_RETURN)
	{
		result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <expression>
		ast_node_t *return_value_node;
		result = expression_opt(cdata, &return_value_node);
		if (result)
			return result;

		statement_node = ast_return_call_make(return_value_node);
		if (!statement_node)
		{
			print_err_l("Failed to create an AST node in statement().", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}
		return ast_block_add_statement(block_node, statement_node);
	}
	// identifier
	else if (cdata->token.type == TOKEN_IDENTIFIER)
	{
		// A lookahead needs to be performed here in order to
		// tell between the rules:
		// id = <expression>
		// <expression>

		token_t id_token = cdata->token;
		token_t next_token;

		result = peek_next_token(&next_token, EOL_FORBIDDEN);
		if (result)
			return result;

		// id = <expression>
		if (next_token.type == TOKEN_ASSIGN)
		{
			result = get_next_token(&cdata->token, EOL_FORBIDDEN);
			if (result)
				return result;

			// Create node with the assigned value, wrap it in an assignment node, then add it to the block
			ast_node_t *assigned_expression_node;

			result = expression(cdata, &assigned_expression_node);
			if (result)
				return result;

			if (!symtable_item_defined(cdata->local_var_table, id_token.attribute.string))
			{
				print_err_l("Variable must be defined before assignment.", ERR_SEMANTIC_UNDEFINED, cdata->token.line);
				return ERR_SEMANTIC_UNDEFINED;
			}

			statement_node = ast_var_assign_make(id_token.attribute.string, assigned_expression_node);
			if (!statement_node)
			{
				print_err_l("Failed to create an AST node in statement().", ERR_INTERNAL, cdata->token.line);
				return ERR_INTERNAL;
			}
			return ast_block_add_statement(block_node, statement_node);
		}

		// <expression>
		result = expression(cdata, &statement_node);
		if (result)
			return result;
		return ast_block_add_statement(block_node, statement_node);
	}

	// <expression>
	result = expression(cdata, &statement_node);
	if (result)
		return result;

	return ast_block_add_statement(block_node, statement_node);
}

/**
 * @brief Parses the statement in a return call. If there is no statement returns a node with the Unit type
 *
 * @param cdata Compiler data
 * @param return_node Address to which the address of the created node will be saved
 * @return err_t Erroc code
 */
err_t expression_opt(compiler_data_t *cdata, ast_node_t **return_node)
{
	// If the token after return is on another line, the return is empty
	if (cdata->token.starts_logical_line || cdata->token.type == TOKEN_EOF)
	{
		literal_t lit;
		lit.literal_type = TYPE_UNIT;

		*return_node = ast_literal_make(&lit);
		if (*return_node == NULL)
		{
			print_err_l("Failed to create an AST node inside expression_opt()", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}

		return ERR_OK;
	}

	return expression(cdata, return_node);
}

/**
 * @brief Optional else branch of an if, later can be made into an else if branch | <else-opt>
 *
 * @param cdata Compiler data
 * @param out_node Pointer to where the pointer to the resulting node should be written.
 *                 NULL will be written if there is no else branch.
 * @param header_indent_level Indentation level of the if statement headers (if, else if, else)
 * @return err_t Error code
 */
err_t else_opt(compiler_data_t *cdata, ast_node_t **out_node, unsigned header_indent_level)
{
	// else
	if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_ELSE &&
		cdata->token.indent == header_indent_level)
	{
		err_t result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <else-if-opt>
		return else_if_opt(cdata, out_node, header_indent_level);
	}

	// Epsilon rule
	*out_node = NULL;
	return ERR_OK;
}

/**
 * @brief Else, or else if branch of an if statement | <else-if-opt>
 *
 * @param cdata Compiler data
 * @param out_node Pointer to where the pointer to the resulting node should be written.
 * @param header_indent_level Indentation level of the if statement headers (if, else if, else)
 * @return err_t Error code
 */
err_t else_if_opt(compiler_data_t *cdata, ast_node_t **out_node, unsigned header_indent_level)
{
	// if (makes for an else if with the previous rule)
	if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_IF)
	{
		err_t result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// <expression>
		ast_node_t *condition_node;
		result = expression(cdata, &condition_node);
		if (result)
			return result;

		// Check if the condition expression results in a boolean
		if (condition_node->eval_type != TYPE_BOOLEAN)
		{
			print_err_l("The condition of an if statement must result in a Boolean.", ERR_SEMANTIC_INVALID_TYPE,
						cdata->token.line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		// then
		if (cdata->token.type != TOKEN_KEYWORD || cdata->token.attribute.keyword != KW_THEN)
		{
			print_err_l("Expected a \"then\" keyword after expression.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// Create the AST node
		*out_node = ast_if_make(condition_node);
		if (!*out_node)
		{
			print_err_l("Failed to create an AST node for and if statement!", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}

		// <block-start>
		ast_node_t *then_block_node = ast_block_make();
		if (!then_block_node)
		{
			print_err_l("Failed to create a block node for an if statement!", ERR_INTERNAL, cdata->token.line);
			return ERR_INTERNAL;
		}

		result = block_start(cdata, then_block_node, header_indent_level);
		if (result)
			return result;

		// Set the then branch
		result = ast_if_set_then_block(*out_node, then_block_node);
		if (result)
			return result;

		// Add the else branch. The node will be filled either with a block or an if node.
		// If the node remains NULL, there was no else branch
		ast_node_t *else_node = NULL;

		// <else-opt>
		result = else_opt(cdata, &else_node, header_indent_level);
		if (result)
			return result;

		result = ast_if_set_else_node(*out_node, else_node);
		if (result)
			return result;

		return ERR_OK;
	}

	// <block_start>
	*out_node = ast_block_make();
	if (!*out_node)
	{
		print_err_l("Failed to create a block node for an else branch of an if statement!", ERR_INTERNAL,
					cdata->token.line);
		return ERR_INTERNAL;
	}
	return block_start(cdata, *out_node, header_indent_level);
}

/**
 * @brief For statement generator | <for-generator>
 *
 * @param cdata Compiler data
 * @param for_node AST for statement node
 * @return err_t Error code
 */
err_t for_generator(compiler_data_t *cdata, ast_node_t *for_node)
{
	// identifier
	if (cdata->token.type == TOKEN_IDENTIFIER)
	{
		err_t result;

		// to
		if (!strcmp(cdata->token.attribute.string, "to"))
		{
			result = get_next_token(&cdata->token, EOL_FORBIDDEN);
			if (result)
				return result;

			// <expression>
			ast_node_t *to_node;
			result = expression(cdata, &to_node);
			if (result)
				return result;

			// Add the node to the for statement
			result = ast_for_set_to(for_node, to_node);
			if (result)
				return result;

			// <for-by>
			return for_by(cdata, for_node);
		}
		// until
		else if (!strcmp(cdata->token.attribute.string, "until"))
		{
			result = get_next_token(&cdata->token, EOL_FORBIDDEN);
			if (result)
				return result;

			// <expression>
			ast_node_t *until_node;
			result = expression(cdata, &until_node);
			if (result)
				return result;

			result = ast_for_set_until(for_node, until_node);
			if (result)
				return result;

			return ast_for_set_type(for_node, FOR_UNTIL);
		}
	}

	print_err_l("Expected \"to\" or \"until\" as a for statement generator.", ERR_SYNTAX, cdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief For statement generator "by" | <for-by>
 *
 * @param cdata Compiler data
 * @param for_node AST for statement node
 * @return err_t Error code
 */
err_t for_by(compiler_data_t *cdata, ast_node_t *for_node)
{
	err_t result;

	// by
	if (cdata->token.type == TOKEN_IDENTIFIER && !strcmp(cdata->token.attribute.string, "by"))
	{
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *by_node;
		result = expression(cdata, &by_node);
		if (result)
			return result;

		result = ast_for_set_by(for_node, by_node);
		if (result)
			return result;

		return ast_for_set_type(for_node, FOR_TO_BY);
	}

	// Epsilon rule
	result = ast_for_set_type(for_node, FOR_TO);
	if (result)
		return result;

	return ERR_OK;
}

/**
 * @brief For statement guard | <for-guard>
 *
 * @param cdata Compiler data
 * @param for_node AST for statement node
 * @return err_t Error code
 */
err_t for_guard(compiler_data_t *cdata, ast_node_t *for_node)
{
	// if
	if (cdata->token.type == TOKEN_KEYWORD && cdata->token.attribute.keyword == KW_IF)
	{
		err_t result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *condition_node;
		result = expression(cdata, &condition_node);
		if (result)
			return result;

		result = ast_for_add_guard(for_node, condition_node);
		if (result)
			return result;

		// <for-guard>
		return for_guard(cdata, for_node);
	}

	// Epsilon rule
	return ERR_OK;
}

/**
 * @brief Val or var definition
 *
 * @param cdata Compiler data
 * @param var_def_node AST variable definition node
 * @param table_entry Entry in the lcoal symbol table of
 * @return err_t Error code
 */
err_t val_var_def(compiler_data_t *cdata, ast_node_t *var_def_node, symbol_data_t *table_entry)
{
	err_t result;

	// :
	if (cdata->token.type == TOKEN_COLON)
	{
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <type>
		result = type(cdata, &table_entry->data.variable.type);
		if (result)
			return result;

		// =
		if (cdata->token.type != TOKEN_ASSIGN)
		{
			print_err_l("Expected an assignment operator after type declaration.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *assigned_value_node;
		result = expression(cdata, &assigned_value_node);
		if (result)
			return result;

		// If types dont match, throw a semantic error
		if (table_entry->data.variable.type != assigned_value_node->eval_type)
		{
			print_err_l("Type of the expression assigned to variable does not match its type.",
						ERR_SEMANTIC_INVALID_TYPE, cdata->token.line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		return ast_var_def_set_assignment(var_def_node, assigned_value_node);
	}

	// =
	if (cdata->token.type == TOKEN_ASSIGN)
	{
		result = get_next_token(&cdata->token, EOL_FORBIDDEN);
		if (result)
			return result;

		// <expression>
		ast_node_t *assigned_value_node;
		result = expression(cdata, &assigned_value_node);
		if (result)
			return result;

		// Set the type of the variable
		table_entry->data.variable.type = assigned_value_node->eval_type;

		return ast_var_def_set_assignment(var_def_node, assigned_value_node);
	}

	print_err_l("Expected a type declaration or an assignment operator after identifier", ERR_SYNTAX,
				cdata->token.line);
	return ERR_SYNTAX;
}

/**
 * @brief Expression rule | <expression>
 *
 * @param cdata Compiler data
 * @param return_node Address to which the address of the created AST note should be written
 * @return err_t Error code
 */
err_t expression(compiler_data_t *cdata, ast_node_t **return_node)
{
	return parse_expression(cdata, return_node);
}

err_t parse(ast_node_t *program_node, compiler_data_t *cdata)
{
	err_t result = prologue(cdata, program_node);

	return result;
}
