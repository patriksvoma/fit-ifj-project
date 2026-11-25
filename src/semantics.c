/**
 * @file semantics.c
 * @brief Additional semantic checks
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-21
 */

#include "semantics.h"

err_t check_literal_int(const ast_node_t *node, const compiler_data_t *cdata);
err_t check_literal_double(const ast_node_t *node, const compiler_data_t *cdata);
err_t check_literal_string(const ast_node_t *node, const compiler_data_t *cdata);
err_t check_literal_boolean(const ast_node_t *node, const compiler_data_t *cdata);
err_t check_literal_unit(const ast_node_t *node, const compiler_data_t *cdata);
err_t check_program(ast_node_t *node, compiler_data_t *cdata);
err_t check_func_def(ast_node_t *node, compiler_data_t *cdata);
err_t check_func_call(ast_node_t *node, compiler_data_t *cdata);
err_t check_ext_def(ast_node_t *node, compiler_data_t *cdata);
err_t check_ext_call(ast_node_t *node, compiler_data_t *cdata);
err_t check_block(ast_node_t *node, compiler_data_t *cdata);
err_t check_un_operator(ast_node_t *node, compiler_data_t *cdata);
err_t check_bin_operator(ast_node_t *node, compiler_data_t *cdata);
err_t check_var_def(ast_node_t *node, compiler_data_t *cdata);
err_t check_var_assign(ast_node_t *node, compiler_data_t *cdata);
err_t check_var_use(ast_node_t *node, const compiler_data_t *cdata);
err_t check_return_call(ast_node_t *node, compiler_data_t *cdata);
err_t check_if(ast_node_t *node, compiler_data_t *cdata);
err_t check_while(ast_node_t *node, compiler_data_t *cdata);
err_t check_for(ast_node_t *node, compiler_data_t *cdata);
err_t check_int_to_double(ast_node_t *node, compiler_data_t *cdata);
err_t insert_int_to_double_node(ast_node_t **node);
err_t check_unknown_node(ast_node_t *node, compiler_data_t *cdata);

/**
 * @brief Recursive checks for the literal_int node
 *
 * @param node AST int literal node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_literal_int(const ast_node_t *node, const compiler_data_t *cdata)
{
	if (!node || !cdata)
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
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_literal_double(const ast_node_t *node, const compiler_data_t *cdata)
{
	if (!node || !cdata)
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
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_literal_string(const ast_node_t *node, const compiler_data_t *cdata)
{
	if (!node || !cdata)
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
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_literal_boolean(const ast_node_t *node, const compiler_data_t *cdata)
{
	if (!node || !cdata)
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
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_literal_unit(const ast_node_t *node, const compiler_data_t *cdata)
{
	if (!node || !cdata)
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
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_program(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_program()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	err_t result;

	// Check all function and extension definitions
	for (size_t i = 0; i < node->program.func_def_count; i++)
	{
		result = check_func_def(node->program.func_defs[i], cdata);
		if (result)
			return result;
	}

	for (size_t i = 0; i < node->program.ext_def_count; i++)
	{
		result = check_ext_def(node->program.ext_defs[i], cdata);
		if (result)
			return result;
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the func_def node
 *
 * @param node AST function definition node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_func_def(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_func_def()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (node->func_def.unique_id == NULL)
	{
		print_err("A function definitions unique_id is NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Get the symbol table entry
	symbol_data_t *entry = symtable_item_find(&cdata->function_table, node->func_def.unique_id);
	if (entry == NULL)
	{
		print_err("A function definition exists in the AST, but no in the symbol table. This should not happen.",
				  ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Resolve the block (the return types are used for return statement semantic checks)
	cdata->func_return_type = entry->data.function.return_type;
	cdata->func_return_explicit = cdata->func_return_type != TYPE_NONE;

	err_t result = check_block(node->func_def.block, cdata);
	if (result)
		return result;

	// If the return type is explicit and not Unit, the last statement must match the return type
	if (cdata->func_return_explicit && entry->data.function.return_type != TYPE_UNIT)
	{
		if (entry->data.function.return_type !=
			node->func_def.block->block.statements[node->func_def.block->block.statement_count - 1]->eval_type)
		{
			print_err("The last statement of a function with an explicit return type must evaluate to the return type.",
					  ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
	}

	// If the return type is implicit, it is taken from the last statement
	if (!cdata->func_return_explicit)
	{
		entry->data.function.return_type =
			node->func_def.block->block.statements[node->func_def.block->block.statement_count - 1]->eval_type;
	}

	// Check if it is a main function, if so, check the parameters and return type
	if (entry->data.function.is_main)
	{
		// Not sure if these checks are required by the specs
		if (entry->data.function.param_list_count != 1)
		{
			print_err("The main function must have exactly 1 empty parameter list!", ERR_SEMANTIC_INVALID_ARG);
			return ERR_SEMANTIC_INVALID_ARG;
		}

		if (entry->data.function.param_lists[0]->param_count != 0)
		{
			print_err("The main function must have exactly 1 empty parameter list!", ERR_SEMANTIC_INVALID_ARG);
			return ERR_SEMANTIC_INVALID_ARG;
		}

		if (entry->data.function.return_type != TYPE_UNIT)
		{
			print_err("The return type of the main function must be Unit!", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the func_call node
 *
 * @param node AST function call node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_func_call(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_func_call()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (node->func_call.unique_id == NULL)
	{
		print_err("A function calls unique_id is NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	const symbol_data_t *entry = symtable_item_find(&cdata->function_table, node->func_call.unique_id);
	if (entry == NULL)
	{
		// TODO: Formatted output with the function name
		print_err("A function that was called is not defined.", ERR_SEMANTIC_UNDEFINED);
		return ERR_SEMANTIC_UNDEFINED;
	}

	// Check parameter list count
	if (node->func_call.param_list_count != entry->data.function.param_list_count)
	{
		print_err("A function was called with a different number of parameter lists than defined.",
				  ERR_SEMANTIC_INVALID_ARG);
		return ERR_SEMANTIC_INVALID_ARG;
	}

	// Check all the parameter nodes
	for (size_t i = 0; i < node->func_call.param_list_count; i++)
	{
		param_node_list_t *list = node->func_call.param_lists[i];
		if (list == NULL)
		{
			print_err("A param node list is NULL. This should not happen!", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		// Check parameter count
		if (list->param_count != entry->data.function.param_lists[i]->param_count)
		{
			print_err("A function was called with a different number of parameters than defined.",
					  ERR_SEMANTIC_INVALID_ARG);
			return ERR_SEMANTIC_INVALID_ARG;
		}

		for (size_t j = 0; j < list->param_count; j++)
		{
			ast_node_t *param = list->param_node[j];
			if (param == NULL)
			{
				print_err("A parameter node is NULL. This should not happen!", ERR_INTERNAL);
				return ERR_INTERNAL;
			}

			err_t result = check_unknown_node(param, cdata);
			if (result)
				return result;

			// Check for the parameter type later, as the eval node can be changed during the semantic checks
			// First try to check for an implicit int to double conversion
			if (entry->data.function.param_lists[i]->params[j]->type == TYPE_DOUBLE && param->eval_type == TYPE_INT)
			{
				result = insert_int_to_double_node(&node->func_call.param_lists[i]->param_node[j]);
				if (result)
					return result;
				param = node->func_call.param_lists[i]->param_node[j];
			}

			// Check if the function is defined with TYPE_ANY
			if (entry->data.function.param_lists[i]->params[j]->type == TYPE_ANY)
			{
				continue;
			}

			if (param->eval_type != entry->data.function.param_lists[i]->params[j]->type)
			{
				print_err("A function was called with a parameter of a different type then its defintion.",
						  ERR_SEMANTIC_INVALID_ARG);
				return ERR_SEMANTIC_INVALID_ARG;
			}
		}
	}

	// Set the correct eval type of the node
	node->eval_type = entry->data.function.return_type;

	return ERR_OK;
}

/**
 * @brief Recursive checks for the ext_def node
 *
 * @param node AST extension definition node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_ext_def(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_ext_def()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (node->ext_def.unique_id == NULL)
	{
		print_err("An extension definitions unique_id is NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	return check_block(node->ext_def.block, cdata);
}

/**
 * @brief Recursive checks for the ext_call node
 *
 * @param node AST extension call node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_ext_call(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_ext_call()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (node->ext_call.unique_id == NULL)
	{
		print_err("An extension calls unique_id is NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Resolve the base node first, so its type can be used
	if (node->ext_call.base_node == NULL)
	{
		print_err("An extension calls base node is NULL. This should not happen!", ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	}

	err_t result = check_unknown_node(node->ext_call.base_node, cdata);
	if (result)
		return result;

	if (node->ext_call.base_node->eval_type == TYPE_NONE)
	{
		print_err("Extensions cannot be called on TYPE_NONE!", ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	}

	// Generate the unique id of the extension based on the resolved type
	char *new_id = generate_unique_ext_id(node->ext_call.unique_id, node->ext_call.base_node->eval_type);

	if (new_id == NULL)
	{
		print_err("Failed to allocate memory for an extension call unique_id.", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	free(node->ext_call.unique_id);
	node->ext_call.unique_id = new_id;

	const symbol_data_t *entry = symtable_item_find(&cdata->extension_table, node->ext_call.unique_id);

	if (entry == NULL)
	{
		// TODO: Formatted output with the extension name
		print_err("An extension that was called is not defined.", ERR_SEMANTIC_UNDEFINED);
		return ERR_SEMANTIC_UNDEFINED;
	}

	// Check parameter list count
	if (node->ext_call.param_list_count != entry->data.extension.param_list_count)
	{
		print_err("An extension was called with a different number of parameter lists than defined.",
				  ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	}

	// Check all the parameter nodes
	for (size_t i = 0; i < node->ext_call.param_list_count; i++)
	{
		param_node_list_t *list = node->ext_call.param_lists[i];
		if (list == NULL)
		{
			print_err("A param node list is NULL. This should not happen!", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		// Check parameter count
		if (list->param_count != entry->data.extension.param_lists[i]->param_count)
		{
			print_err("An extension was called with a different number of parameters than defined.",
					  ERR_SEMANTIC_INVALID_ARG);
			return ERR_SEMANTIC_INVALID_ARG;
		}

		for (size_t j = 0; j < list->param_count; j++)
		{
			ast_node_t *param = list->param_node[j];
			if (param == NULL)
			{
				print_err("A parameter node is NULL. This should not happen!", ERR_INTERNAL);
				return ERR_INTERNAL;
			}

			result = check_unknown_node(param, cdata);
			if (result)
				return result;

			// Check for the parameter type later, as the eval node can be changed during the semantic checks
			// First try to check for an implicit int to double conversion
			if (entry->data.extension.param_lists[i]->params[j]->type == TYPE_DOUBLE && param->eval_type == TYPE_INT)
			{
				result = insert_int_to_double_node(&node->ext_call.param_lists[i]->param_node[j]);
				if (result)
					return result;
				param = node->ext_call.param_lists[i]->param_node[j];
			}

			// Check if the extension is defined with TYPE_ANY
			if (entry->data.extension.param_lists[i]->params[j]->type == TYPE_ANY)
			{
				continue;
			}

			if (param->eval_type != entry->data.extension.param_lists[i]->params[j]->type)
			{
				print_err("An extension was called with a parameter of a different type then its defintion.",
						  ERR_SEMANTIC_INVALID_ARG);
				return ERR_SEMANTIC_INVALID_ARG;
			}
		}
	}

	// Set the correct eval type of the node
	node->eval_type = entry->data.extension.return_type;

	return ERR_OK;
}

/**
 * @brief Recursive checks for the block node
 *
 * @param node AST block node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_block(ast_node_t *node, compiler_data_t *cdata)
{
	// TODO: Maybe pass the expected return type/last statement type for checking correct return placement

	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_block()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Check all the statements
	for (size_t i = 0; i < node->block.statement_count; i++)
	{
		if (node->block.statements[i] == NULL)
		{
			print_err("Null statement in a block. This should not happen!", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		err_t result = check_unknown_node(node->block.statements[i], cdata);
		if (result)
			return result;
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the un_operator node
 *
 * @param node AST unary operator node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_un_operator(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_un_operator()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	err_t result = check_unknown_node(node->un_operator.operand, cdata);
	if (result)
		return result;

	switch (node->un_operator.operator)
	{
	case OP_UPLUS:
		if (node->un_operator.operand->eval_type != TYPE_INT && node->un_operator.operand->eval_type != TYPE_DOUBLE)
		{
			print_err("Unary operator + must have an operand of type Int or Double.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_UMINUS:
		if (node->un_operator.operand->eval_type != TYPE_INT && node->un_operator.operand->eval_type != TYPE_DOUBLE)
		{
			print_err("Unary operator - must have an operand of type Int or Double.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_NEG:
		if (node->un_operator.operand->eval_type != TYPE_BOOLEAN)
		{
			print_err("Operator ! must have an operand of type Boolean.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	default:
		print_err("Invalid operator inside a unary operator node. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	node->eval_type = node->un_operator.operand->eval_type;

	return ERR_OK;
}

/**
 * @brief Recursive checks for the bin_operator node
 *
 * @param node AST binary operator node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_bin_operator(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_bin_operator()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Check the operands (this will resolve their eval types)
	err_t result = check_unknown_node(node->bin_operator.operand_left, cdata);
	if (result)
		return result;

	result = check_unknown_node(node->bin_operator.operand_right, cdata);
	if (result)
		return result;

	data_type_t op_left_type = node->bin_operator.operand_left->eval_type;
	data_type_t op_right_type = node->bin_operator.operand_right->eval_type;

	// Do checks based on the operator
	switch (node->bin_operator.operator)
	{
	case OP_MUL:
		if (op_left_type == op_right_type && (op_left_type == TYPE_INT || op_left_type == TYPE_DOUBLE))
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator * must have operands of type Int or Double.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_DIV:
		if (op_left_type == op_right_type && (op_left_type == TYPE_INT || op_left_type == TYPE_DOUBLE))
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator / must have operands of type Int or Double.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_PLUS:
		if (op_left_type == op_right_type &&
			(op_left_type == TYPE_INT || op_left_type == TYPE_DOUBLE || op_left_type == TYPE_STRING))
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator + must have operands of type Int or Double.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_MINUS:
		if (op_left_type == op_right_type && (op_left_type == TYPE_INT || op_left_type == TYPE_DOUBLE))
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator - must have operands of type Int or Double.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_LESS:
		if (op_left_type == op_right_type &&
			(op_left_type == TYPE_INT || op_left_type == TYPE_DOUBLE || op_left_type == TYPE_STRING))
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator < must have operands of type Int, Double or String.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_LESS_EQUAL:
		if (op_left_type == op_right_type &&
			(op_left_type == TYPE_INT || op_left_type == TYPE_DOUBLE || op_left_type == TYPE_STRING))
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator <= must have operands of type Int, Double or String.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_GREATER:
		if (op_left_type == op_right_type &&
			(op_left_type == TYPE_INT || op_left_type == TYPE_DOUBLE || op_left_type == TYPE_STRING))
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator > must have operands of type Int, Double or String.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_GREATER_EQUAL:
		if (op_left_type == op_right_type &&
			(op_left_type == TYPE_INT || op_left_type == TYPE_DOUBLE || op_left_type == TYPE_STRING))
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator >= must have operands of type Int, Double or String.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_EQUAL:
		if (op_left_type == op_right_type)
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator == must have two numeric operands, or operands of the same type.",
					  ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_NOT_EQUAL:
		if (op_left_type == op_right_type)
			break;
		else if (op_left_type == TYPE_INT && op_right_type == TYPE_DOUBLE)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_left);
			if (result)
				return result;
		}
		else if (op_left_type == TYPE_DOUBLE && op_right_type == TYPE_INT)
		{
			result = insert_int_to_double_node(&node->bin_operator.operand_right);
			if (result)
				return result;
		}
		else
		{
			print_err("Operator != must have two numeric operands, or operands of the same type.",
					  ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
		break;
	case OP_LOGIC_AND:
		if (op_left_type == op_right_type && op_left_type == TYPE_BOOLEAN)
			break;
		print_err("Operator && must have operands of type Boolean.", ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	case OP_LOGIC_OR:
		if (op_left_type == op_right_type && op_left_type == TYPE_BOOLEAN)
			break;
		print_err("Operator || must have operands of type Boolean.", ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	default:
		print_err("Invalid operator inside a binary operator node. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	operator_type_t op = node->bin_operator.operator;

	if (op == OP_LESS || op == OP_LESS_EQUAL || op == OP_GREATER || op == OP_GREATER_EQUAL || op == OP_EQUAL ||
		op == OP_NOT_EQUAL)
		node->eval_type = TYPE_BOOLEAN;
	else
		node->eval_type = node->bin_operator.operand_left->eval_type;

	return ERR_OK;
}

/**
 * @brief Recursive checks for the var_def node
 *
 * @param node AST variable definition node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_var_def(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_var_def()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (node->variable_definition.unique_id == NULL)
	{
		print_err("A variable definitions unique_id is NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	err_t result = check_unknown_node(node->variable_definition.assigned_value, cdata);
	if (result)
		return result;

	// Check and set the variable type
	symbol_data_t *entry = node->variable_definition.sym_table_entry;
	if (!entry)
	{
		print_err("A variable definiton AST node is missing the symbol_table_entry record. This shoud not happepen",
				  ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (entry->data.variable.type == TYPE_NONE)
	{
		// Implicit definition
		entry->data.variable.type = node->variable_definition.assigned_value->eval_type;
		if (entry->data.variable.type == TYPE_NONE)
		{
			print_err("An expression assigned to a variable resolved to TYPE_NONE. This should not happen.",
					  ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
	}
	else
	{
		// Explicit definition, check if the types match
		if (entry->data.variable.type != node->variable_definition.assigned_value->eval_type)
		{
			// TODO: Formatted output
			print_err("A variable has an explicit type, but is assigned a value of a different type.",
					  ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
	}

	node->eval_type = TYPE_UNIT;

	return ERR_OK;
}

/**
 * @brief Recursive checks for the var_assign node
 *
 * @param node AST variable assignment node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_var_assign(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_var_assign()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (node->variable_assignment.unique_id == NULL)
	{
		print_err("A variable assignments unique_id is NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	err_t result = check_unknown_node(node->variable_assignment.assigned_value, cdata);
	if (result)
		return result;

	// Check the variable type
	const symbol_data_t *entry = node->variable_assignment.sym_table_entry;
	if (!entry)
	{
		print_err("A variable definiton AST node is missing the symbol_table_entry record. This shoud not happepen",
				  ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (entry->data.variable.type != node->variable_assignment.assigned_value->eval_type)
	{
		print_err("Trying to assign value of a wrong type to a variable.", ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the var_use node
 *
 * @param node AST variable use node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_var_use(ast_node_t *node, const compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_var_use()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (node->variable_use.unique_id == NULL)
	{
		print_err("A variable use unique_id is NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Use the variable type as the new eval_type
	symbol_data_t *entry = node->variable_use.sym_table_entry;
	if (!entry)
	{
		print_err("A variable definiton AST node is missing the symbol_table_entry record. This shoud not happepen",
				  ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	node->eval_type = entry->data.variable.type;

	return ERR_OK;
}

/**
 * @brief Recursive checks for the return_call node
 *
 * @param node AST return call node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_return_call(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_return_call()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	err_t result = check_unknown_node(node->return_call, cdata);
	if (result)
		return result;

	if (!cdata->func_return_explicit)
	{
		print_err("Return statements are not allowed in functions with implict return types.",
				  ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	}

	if (cdata->func_return_type != TYPE_UNIT)
	{
		if (cdata->func_return_type != node->return_call->eval_type)
		{
			print_err("Expression inside a return statement must match the functions return type.",
					  ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the if node
 *
 * @param node AST if node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_if(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_if()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	err_t result = check_unknown_node(node->if_statement.condition, cdata);
	if (result)
		return result;

	// Check the condition evaluation type
	if (node->if_statement.condition->eval_type != TYPE_BOOLEAN)
	{
		print_err("The condition of an if statement must evaluate to a Boolean.", ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	}

	result = check_unknown_node(node->if_statement.if_block, cdata);
	if (result)
		return result;

	// The else block can be null
	if (node->if_statement.else_block)
	{
		result = check_unknown_node(node->if_statement.else_block, cdata);
		if (result)
			return result;
	}

	return ERR_OK;
}

/**
 * @brief Recursive checks for the while node
 *
 * @param node AST while node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_while(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_while()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	err_t result = check_unknown_node(node->while_loop.condition, cdata);
	if (result)
		return result;

	// Check the condition evaluation type
	if (node->while_loop.condition->eval_type != TYPE_BOOLEAN)
	{
		print_err("The condition of a while loop must evaluate to a Boolean.", ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	}

	result = check_unknown_node(node->while_loop.block, cdata);
	if (result)
		return result;

	return ERR_OK;
}

/**
 * @brief Recursive checks for the for node
 *
 * @param node AST for node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_for(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_for()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (node->for_loop.iterator_unique_id == NULL)
	{
		print_err("Unique id of a for loop iterator is NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Check the "from" node
	err_t result = check_unknown_node(node->for_loop.from, cdata);
	if (result)
		return result;

	if (node->for_loop.from->eval_type != TYPE_INT)
	{
		print_err("The \"from\" generator of a for loop must be type Int.", ERR_SEMANTIC_INVALID_TYPE);
		return ERR_SEMANTIC_INVALID_TYPE;
	}

	// Check other nodes based on the type of the for loop
	switch (node->for_loop.for_type)
	{
	case FOR_TO:
		// to
		result = check_unknown_node(node->for_loop.to, cdata);
		if (result)
			return result;

		if (node->for_loop.to->eval_type != TYPE_INT)
		{
			print_err("The \"to\" generator of a for loop must be type Int.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		break;
	case FOR_UNTIL:
		// until
		result = check_unknown_node(node->for_loop.until, cdata);
		if (result)
			return result;

		if (node->for_loop.until->eval_type != TYPE_INT)
		{
			print_err("The \"until\" generator of a for loop must be type Int.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		break;
	case FOR_TO_BY:
		// to
		result = check_unknown_node(node->for_loop.to, cdata);
		if (result)
			return result;

		if (node->for_loop.to->eval_type != TYPE_INT)
		{
			print_err("The \"to\" generator of a for loop must be type Int.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		// by
		result = check_unknown_node(node->for_loop.by, cdata);
		if (result)
			return result;

		if (node->for_loop.by->eval_type != TYPE_INT)
		{
			print_err("The \"by\" generator of a for loop must be type Int.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		// TODO: Should be checked for all literals, so probably also for unary operators and simple expressions
		// Could be done by having a "evaluates_to_zero" flag for a node
		if (node->for_loop.by->node_type == AST_LITERAL_INT && node->for_loop.by->literal_int == 0)
		{
			print_err("The step of a for loop generator must not be 0.", ERR_SEMANTIC_OTHER);
			return ERR_SEMANTIC_OTHER;
		}

		break;
	default:
		print_err("Invalid for loop type. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Check all the guards
	for (size_t i = 0; i < node->for_loop.guard_count; i++)
	{
		ast_node_t *guard = node->for_loop.guards[i];
		if (guard == NULL)
		{
			print_err("A for loop guard is NULL. This should not happen!", ERR_INTERNAL);
			return ERR_INTERNAL;
		}

		result = check_unknown_node(guard, cdata);
		if (result)
			return result;

		if (guard->eval_type != TYPE_BOOLEAN)
		{
			print_err("The guard of a for loop must evaluate to a Boolean.", ERR_SEMANTIC_INVALID_TYPE);
			return ERR_SEMANTIC_INVALID_TYPE;
		}
	}

	// Check the block
	result = check_block(node->for_loop.block, cdata);
	if (result)
		return result;

	return ERR_OK;
}

/**
 * @brief Recursive checks for the int_to_double node
 *
 * @param node AST in to double conversion node
 * @param cdata Compiler data
 * @return err_t Error code
 */
err_t check_int_to_double(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_int_to_double()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (node->int2double == NULL)
	{
		print_err("The inside node of a int-to-double node is NULL. This should not happen!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	return check_unknown_node(node->int2double, cdata);
}

/**
 * @brief Wraps an Integer type AST node in a int-to-double node
 *
 * @param node Pointer to where the adress of the new node should be written
 * @return err_t Error code
 */
err_t insert_int_to_double_node(ast_node_t **node)
{
	if (!node || !*node)
	{
		print_err("Null-pointer reference in insert_in_to_double_node().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if ((*node)->eval_type != TYPE_INT)
	{
		print_err("Tried to create an int-to-double node on a node with a different type than Int. This should not "
				  "happen!",
				  ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	*node = ast_int2double_make(*node);
	if (*node == NULL)
	{
		print_err("Failed to create an int-to-double AST node.", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	return ERR_OK;
}

/**
 * @brief Calls the correct check function based on the node type.
 *        Used for checks where a child can by any node
 *
 * @param node
 * @param cdata
 * @return err_t
 */
err_t check_unknown_node(ast_node_t *node, compiler_data_t *cdata)
{
	if (!node || !cdata)
	{
		print_err("Null-pointer reference in check_unknown_node()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	switch (node->node_type)
	{
	case AST_LITERAL_INT:
		return check_literal_int(node, cdata);
	case AST_LITERAL_DOUBLE:
		return check_literal_double(node, cdata);
	case AST_LITERAL_STRING:
		return check_literal_string(node, cdata);
	case AST_LITERAL_BOOLEAN:
		return check_literal_boolean(node, cdata);
	case AST_LITERAL_UNIT:
		return check_literal_unit(node, cdata);
	case AST_PROGRAM:
		return check_program(node, cdata);
	case AST_FUNC_DEF:
		return check_func_def(node, cdata);
	case AST_FUNC_CALL:
		return check_func_call(node, cdata);
	case AST_EXT_DEF:
		return check_ext_def(node, cdata);
	case AST_EXT_CALL:
		return check_ext_call(node, cdata);
	case AST_BLOCK:
		return check_block(node, cdata);
	case AST_UN_OPERATOR:
		return check_un_operator(node, cdata);
	case AST_BIN_OPERATOR:
		return check_bin_operator(node, cdata);
	case AST_VAR_DEF:
		return check_var_def(node, cdata);
	case AST_VAR_ASSIGN:
		return check_var_assign(node, cdata);
	case AST_VAR_USE:
		return check_var_use(node, cdata);
	case AST_RETURN_CALL:
		return check_return_call(node, cdata);
	case AST_IF:
		return check_if(node, cdata);
	case AST_WHILE:
		return check_while(node, cdata);
	case AST_FOR:
		return check_for(node, cdata);
	case AST_INT_TO_DOUBLE:
		return check_int_to_double(node, cdata);
	default:
		print_err("Error in check_unknown_node()", ERR_INTERNAL);
		return ERR_INTERNAL;
	}
}

err_t do_semantic_checks(ast_node_t *program_node, compiler_data_t *cdata)
{
	if (!cdata || !program_node)
	{
		print_err("Null-pointer exception in do_semantic_checks()!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (!cdata->main_defined)
	{
		print_err("No main function is defined!", ERR_SEMANTIC_UNDEFINED);
		return ERR_SEMANTIC_UNDEFINED;
	}

	// Run a recursive check
	return check_program(program_node, cdata);
}
