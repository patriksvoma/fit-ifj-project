/**
 * @file semantics.c
 * @brief Additional semantic checks
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-21
 */

#include "semantics.h"

err_t check_literal_int(ast_node_t *node, parser_data_t *pdata);
err_t check_literal_double(ast_node_t *node, parser_data_t *pdata);
err_t check_literal_string(ast_node_t *node, parser_data_t *pdata);
err_t check_literal_boolean(ast_node_t *node, parser_data_t *pdata);
err_t check_literal_unit(ast_node_t *node, parser_data_t *pdata);
err_t check_program(ast_node_t *node, parser_data_t *pdata);
err_t check_func_def(ast_node_t *node, parser_data_t *pdata);
err_t check_func_call(ast_node_t *node, parser_data_t *pdata);
err_t check_ext_def(ast_node_t *node, parser_data_t *pdata);
err_t check_ext_call(ast_node_t *node, parser_data_t *pdata);
err_t check_block(ast_node_t *node, parser_data_t *pdata);
err_t check_un_operator(ast_node_t *node, parser_data_t *pdata);
err_t check_bin_operator(ast_node_t *node, parser_data_t *pdata);
err_t check_var_def(ast_node_t *node, parser_data_t *pdata);
err_t check_var_assign(ast_node_t *node, parser_data_t *pdata);
err_t check_var_use(ast_node_t *node, parser_data_t *pdata);
err_t check_return_call(ast_node_t *node, parser_data_t *pdata);
err_t check_if(ast_node_t *node, parser_data_t *pdata);
err_t check_while(ast_node_t *node, parser_data_t *pdata);
err_t check_for(ast_node_t *node, parser_data_t *pdata);
err_t check_int_to_double(ast_node_t *node, parser_data_t *pdata);

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_int(ast_node_t *node, parser_data_t *pdata)
{
	// TODO: Implement
	(void)node;
	(void)pdata;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_double(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_string(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_boolean(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_unit(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

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
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_func_def(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_func_call(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_ext_def(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_ext_call(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_block(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_un_operator(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_bin_operator(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_var_def(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_var_assign(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_var_use(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_return_call(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_if(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_while(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_for(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the program node
 *
 * @param node AST program node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_int_to_double(ast_node_t *node, parser_data_t *pdata)
{
	(void)pdata;
	(void)node;
	return ERR_OK;
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
