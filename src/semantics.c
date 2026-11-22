/**
 * @file semantics.c
 * @brief Additional semantic checks
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-21
 */

#include "semantics.h"

err_t check_literal_int(const ast_node_t *node, const parser_data_t *pdata);
err_t check_literal_double(const ast_node_t *node, const parser_data_t *pdata);
err_t check_literal_string(const ast_node_t *node, const parser_data_t *pdata);
err_t check_literal_boolean(const ast_node_t *node, const parser_data_t *pdata);
err_t check_literal_unit(const ast_node_t *node, const parser_data_t *pdata);
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
 * @brief Recursive checks for the literal_int node
 *
 * @param node AST int literal node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_int(const ast_node_t *node, const parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_literal_int()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the literal_double node
 *
 * @param node AST double literal node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_double(const ast_node_t *node, const parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_literal_double()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the literal_string node
 *
 * @param node AST string literal node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_string(const ast_node_t *node, const parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_literal_string()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// The pointer to the string must not be null
	if (node->literal_string == NULL)
	{
		print_err("A string literal must not be NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the literal_boolean node
 *
 * @param node AST boolean literal node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_boolean(const ast_node_t *node, const parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_literal_boolean()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the literal_unit node
 *
 * @param node AST unit literal node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_literal_unit(const ast_node_t *node, const parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_literal_unit()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

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
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_program()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the func_def node
 *
 * @param node AST function definition node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_func_def(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_func_def()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the func_call node
 *
 * @param node AST function call node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_func_call(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_func_call()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the ext_def node
 *
 * @param node AST extension definition node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_ext_def(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_ext_def()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the ext_call node
 *
 * @param node AST extension call node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_ext_call(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_ext_call()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the block node
 *
 * @param node AST block node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_block(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_block()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the un_operator node
 *
 * @param node AST unary operator node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_un_operator(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_un_operator()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the bin_operator node
 *
 * @param node AST binary operator node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_bin_operator(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_bin_operator()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the var_def node
 *
 * @param node AST variable definition node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_var_def(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_var_def()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the var_assign node
 *
 * @param node AST variable assignment node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_var_assign(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_var_assign()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the var_use node
 *
 * @param node AST variable use node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_var_use(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_var_use()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the return_call node
 *
 * @param node AST return call node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_return_call(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_return_call()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the if node
 *
 * @param node AST if node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_if(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_if()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the while node
 *
 * @param node AST while node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_while(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_while()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the for node
 *
 * @param node AST for node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_for(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_for()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
	(void)pdata;
	(void)node;
	return ERR_OK;
}

/**
 * @brief Recursive checks for the int_to_double node
 *
 * @param node AST in to double conversion node
 * @param pdata Parser data
 * @return err_t Error code
 */
err_t check_int_to_double(ast_node_t *node, parser_data_t *pdata)
{
	if (!node || !pdata)
	{
		print_err("Null-pointer reference in check_int_to_double()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// TODO: Implement
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
