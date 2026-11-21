/**
 * @file semantics.c
 * @brief Additional semantic checks
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-21
 */

#include "semantics.h"

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_program(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;

	// for (size_t i = 0; i < node->program.func_def_count; i++)
	// 	check_func_def()
}

err_t do_semantic_checks(ast_node_t *program_node, parser_data_t *pdata)
{
	if (!pdata || !program_node)
	{
		print_err("Null-pointer exception in do_semantic_checks()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (!pdata->main_defined)
	{
		print_err("No main function is defined!", ERR_SEMANTIC_UNDEFINED);
		return ERR_SEMANTIC_UNDEFINED;
	}

	// Run a recursive check
	return check_program(program_node, pdata);
}
