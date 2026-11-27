/**
 * @file expression.h
 * @brief Operator-Precedence part of the syntax analyzer in the IFJ26 compiler, used for parsing expressions
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-14
 */

#include "expression.h"
#include "symstack.h"
#include <string.h>

#define PREC_TABLE_SIZE 20

static err_t parse_expression_internal(compiler_data_t *cdata, ast_node_t **return_node, bool stop_at_call_delimiter);
static err_t parse_function_call(compiler_data_t *cdata, const token_t *identifier_token, ast_node_t **return_node);
static err_t try_parse_if_expression(compiler_data_t *cdata, symstack_item_t *item, bool expect_operand,
									 bool stop_at_closing_parenthesis, bool *expression_consumed);

/**
 * @brief All possible grammar rules
 *
 */
typedef enum
{
	RULE_NONE,		  // Invalid rule
	RULE_I,			  // E :: i
	RULE_PARENTHESIS, // E :: (E)
	RULE_UNIT,		  // E :: ()
	RULE_DOT,		  // E :: E.E
	RULE_NEG,		  // E :: !E
	RULE_UPLUS,		  // E :: +E
	RULE_UMINUS,	  // E :: -E
	RULE_MUL,		  // E :: E*E
	RULE_DIV,		  // E :: E/E
	RULE_ADD,		  // E :: E+E
	RULE_SUB,		  // E :: E-E
	RULE_LT,		  // E :: E<E
	RULE_LE,		  // E :: E<=E
	RULE_GT,		  // E :: E>E
	RULE_GE,		  // E :: E>=
	RULE_EQ,		  // E :: E==E
	RULE_NE,		  // E :: E!=E
	RULE_AND,		  // E :: E&&E
	RULE_OR			  // E :: E||E
} prec_rule_t;

/**
 * @brief Precedence table
 *
 */
prec_table_operator_t prec_table[PREC_TABLE_SIZE][PREC_TABLE_SIZE] = {
	{N, N, R, R, N, N, N, R, R, R, R, R, R, R, R, R, R, R, R, R},
	{S, S, E, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, N},
	{N, N, R, R, N, N, N, R, R, R, R, R, R, R, R, R, R, R, R, R},
	{S, N, R, R, R, R, R, R, R, R, R, R, R, R, R, R, R, R, R, R},
	{S, S, R, S, S, S, S, R, R, R, R, R, R, R, R, R, R, R, R, R},
	{S, S, R, S, S, S, S, R, R, R, R, R, R, R, R, R, R, R, R, R},
	{S, S, R, S, S, S, S, R, R, R, R, R, R, R, R, R, R, R, R, R},
	{S, S, R, S, S, S, S, R, R, R, R, R, R, R, R, R, R, R, R, R},
	{S, S, R, S, S, S, S, R, R, R, R, R, R, R, R, R, R, R, R, R},
	{S, S, R, S, S, S, S, S, S, R, R, R, R, R, R, R, R, R, R, R},
	{S, S, R, S, S, S, S, S, S, R, R, R, R, R, R, R, R, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, R, R, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, R, R, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, R, R, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, R, R, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, S, S, S, S, N, N, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, S, S, S, S, N, N, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, S, S, S, S, S, S, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, R, R},
	{S, S, N, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, N}};

/**
 * @brief Fills a stack item with data from a token
 *
 * @param token Currently parsed token
 * @param stack_item Stack item which will be filled
 * @param expect_unary Pointer to a variable which is used to determine between unary and binary +/-.
 * 					   Value of this variable is changed based on the token type.
 * @return err_t Error code
 */
err_t token_to_stack_item(const token_t *token, symstack_item_t *stack_item, bool *expect_unary)
{
	if (!token || !stack_item)
	{
		print_err("Null-reference error in token_to_stack_item().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	symstack_item_t item;
	item.next = NULL;
	item.source_token = *token;
	item.node = NULL;
	// TODO: Maybe copy the string if the token is an identificator

	switch (token->type)
	{
	case TOKEN_IDENTIFIER:
		// If the identifier is to, until, or by, end the expression
		if (!strcmp(token->attribute.string, "to") || !strcmp(token->attribute.string, "until") ||
			!strcmp(token->attribute.string, "by"))
		{
			item.type = STACKITEM_TYPE_TERMINAL;
			item.data.terminal = TERMINAL_DOLLAR;
		}
		else
		{
			item.type = STACKITEM_TYPE_TERMINAL;
			item.data.terminal = TERMINAL_I;
		}
		*expect_unary = false;
		break;
	case TOKEN_KEYWORD:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		// TODO: Support if statements
		break;
	case TOKEN_OPENING_BRACKET:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_LEFT_BRACKET;
		*expect_unary = true;
		break;
	case TOKEN_CLOSING_BRACKET:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_RIGHT_BRACKET;
		*expect_unary = false;
		break;
	case TOKEN_BOOLEAN:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_I;
		*expect_unary = false;
		break;
	case TOKEN_INT:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_I;
		*expect_unary = false;
		break;
	case TOKEN_DOUBLE:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_I;
		*expect_unary = false;
		break;
	case TOKEN_STRING:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_I;
		*expect_unary = false;
		break;
	case TOKEN_EOF:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		break;
	case TOKEN_FOR_GEN_ASSIGN:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		break;
	case TOKEN_DOT:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOT;
		*expect_unary = false;
		break;
	case TOKEN_COMMA:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		break;
	case TOKEN_COLON:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		break;
	case TOKEN_UNDERSCORE:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		break;
	case TOKEN_PLUS:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = *expect_unary ? TERMINAL_U_PLUS : TERMINAL_ADD;
		*expect_unary = true;
		break;
	case TOKEN_MINUS:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = *expect_unary ? TERMINAL_U_MINUS : TERMINAL_SUB;
		*expect_unary = true;
		break;
	case TOKEN_ASTERISK:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_MUL;
		*expect_unary = true;
		break;
	case TOKEN_DIV:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DIV;
		*expect_unary = true;
		break;
	case TOKEN_ASSIGN:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		break;
	case TOKEN_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_EQ;
		*expect_unary = true;
		break;
	case TOKEN_NOT_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_NE;
		*expect_unary = true;
		break;
	case TOKEN_GREATER:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_GT;
		*expect_unary = true;
		break;
	case TOKEN_GREATER_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_GET;
		*expect_unary = true;
		break;
	case TOKEN_LESS:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_LT;
		*expect_unary = true;
		break;
	case TOKEN_LESS_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_LET;
		*expect_unary = true;
		break;
	case TOKEN_AND:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_AND;
		*expect_unary = true;
		break;
	case TOKEN_OR:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_OR;
		*expect_unary = true;
		break;
	case TOKEN_NEG:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_U_NEG;
		*expect_unary = true;
		break;
	case TOKEN_SEMICOLON:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		break;
	default:
		print_err_l("Invalid token type in expression.", ERR_SYNTAX, token->line);
		return ERR_SYNTAX;
	}

	*stack_item = item;
	return ERR_OK;
}

/**
 * @brief Checks if any rule is applicable to the symbols on the top of the stack
 *
 * @param stack Symbol stack
 * @return prec_rule_t Found rule
 */
prec_rule_t find_rule(const symstack_t *stack)
{
	if (!stack || !stack->top || !stack->top->next)
		return RULE_NONE;

	const symstack_item_t *top1 = stack->top;
	const symstack_item_t *top2 = top1->next;
	const symstack_item_t *top3 = NULL;
	const symstack_item_t *top4 = NULL;

	if (top2)
		top3 = top2->next;
	if (top3)
		top4 = top3->next;

	// <i
	if (top1->type == STACKITEM_TYPE_TERMINAL && top1->data.terminal == TERMINAL_I)
	{
		if (top2->type == STACKITEM_TYPE_OPERATOR && top2->data.operator == S)
		{
			return RULE_I;
		}
	}
	// <(E) or <()
	else if (top1->type == STACKITEM_TYPE_TERMINAL && top1->data.terminal == TERMINAL_RIGHT_BRACKET)
	{
		if (!top3)
			return RULE_NONE;

		// <()
		if (top2->type == STACKITEM_TYPE_TERMINAL && top2->data.terminal == TERMINAL_LEFT_BRACKET &&
			top3->type == STACKITEM_TYPE_OPERATOR && top3->data.operator == S)
		{
			return RULE_UNIT;
		}

		if (!top4)
			return RULE_NONE;

		if (top2->type == STACKITEM_TYPE_EXPRESSION && top3->type == STACKITEM_TYPE_TERMINAL &&
			top3->data.terminal == TERMINAL_LEFT_BRACKET && top4->type == STACKITEM_TYPE_OPERATOR &&
			top4->data.operator == S)
		{
			return RULE_PARENTHESIS;
		}
	}
	// xxE
	else if (top1->type == STACKITEM_TYPE_EXPRESSION && top2->type == STACKITEM_TYPE_TERMINAL)
	{
		// <ExE
		if (top3 && top4 && top3->type == STACKITEM_TYPE_EXPRESSION && top4->type == STACKITEM_TYPE_OPERATOR &&
			top4->data.operator == S)
		{
			switch (top2->data.terminal)
			{
			// <E.E
			case TERMINAL_DOT:
				return RULE_DOT;
			// <E*E
			case TERMINAL_MUL:
				return RULE_MUL;
			// <E/E
			case TERMINAL_DIV:
				return RULE_DIV;
			// <E+E
			case TERMINAL_ADD:
				return RULE_ADD;
			// <E-E
			case TERMINAL_SUB:
				return RULE_SUB;
			// <E<E
			case TERMINAL_LT:
				return RULE_LT;
			// <E<=E
			case TERMINAL_LET:
				return RULE_LE;
			// <E>E
			case TERMINAL_GT:
				return RULE_GT;
			// <E>=E
			case TERMINAL_GET:
				return RULE_GE;
			// <E==E
			case TERMINAL_EQ:
				return RULE_EQ;
			// <E!=E
			case TERMINAL_NE:
				return RULE_NE;
			// <E&&E
			case TERMINAL_AND:
				return RULE_AND;
			// <E||E
			case TERMINAL_OR:
				return RULE_OR;
			default:
				return RULE_NONE;
			}
		}
		// <xE
		else if (top3 && top3->type == STACKITEM_TYPE_OPERATOR && top3->data.operator == S)
		{
			switch (top2->data.terminal)
			{
			// <!E
			case TERMINAL_U_NEG:
				return RULE_NEG;
			// <+E
			case TERMINAL_U_PLUS:
				return RULE_UPLUS;
			// <-E
			case TERMINAL_U_MINUS:
				return RULE_UMINUS;
			default:
				return RULE_NONE;
			}
		}
	}

	return RULE_NONE;
}

/**
 * @brief Applies a rule on the stack by popping the necessary symbols and pushing a new expression one
 *
 * @param rule Rule to apply
 * @param stack Symbol stack
 * @param cdata Compiler data
 * @param line Line on which the expression is located
 * @return err_t Error code
 */
err_t apply_rule(prec_rule_t rule, symstack_t *stack, compiler_data_t *cdata, unsigned line)
{
	err_t result;

	ast_node_t *node = NULL;

	switch (rule)
	{
	// i
	case RULE_I:
		token_t token = stack->top->source_token;
		// If the stack item has a node, it is a parsed function call, and the node can be reused
		if (stack->top->node)
		{
			node = stack->top->node;
			stack->top->node = NULL;
		}
		else if (token.type == TOKEN_BOOLEAN)
		{
			literal_t lit;
			lit.literal_type = TYPE_BOOLEAN;
			lit.value.boolean = token.attribute.boolean;

			node = ast_literal_make(&lit);
		}
		else if (token.type == TOKEN_INT)
		{
			literal_t lit;
			lit.literal_type = TYPE_INT;
			lit.value.integer = token.attribute.integer;

			node = ast_literal_make(&lit);
		}
		else if (token.type == TOKEN_DOUBLE)
		{
			literal_t lit;
			lit.literal_type = TYPE_DOUBLE;
			lit.value.decimal = token.attribute.decimal;

			node = ast_literal_make(&lit);
		}
		else if (token.type == TOKEN_STRING)
		{
			literal_t lit;
			lit.literal_type = TYPE_STRING;
			lit.value.string = token.attribute.string;

			node = ast_literal_make(&lit);
		}
		else if (token.type == TOKEN_IDENTIFIER)
		{
			// Find the identifier_token in the symbol table
			symbol_data_t *entry = symtable_item_find(cdata->local_var_table, token.attribute.string);
			// If the identifier exists, it is either a variable or a function call
			if (entry)
			{
				if (entry->symbol_type == SYM_VARIABLE)
				{
					if (!entry->data.variable.is_defined)
					{
						print_err_l("Variable is not defined.", ERR_SEMANTIC_UNDEFINED, token.line);
						return ERR_SEMANTIC_UNDEFINED;
					}
					node = ast_var_use_make(entry->data.variable.unique_name, entry->data.variable.type);
					if (!node)
					{
						print_err_l("Failed to create an AST node in apply_rule()!", ERR_INTERNAL, line);
						return ERR_INTERNAL;
					}
					node->variable_use.sym_table_entry = entry;
				}
				else if (entry->symbol_type == SYM_FUNCTION)
				{
					if (!entry->data.function.is_defined)
					{
						print_err_l("Function is not defined.", ERR_SEMANTIC_UNDEFINED, token.line);
						return ERR_SEMANTIC_UNDEFINED;
					}

					// Create the AST node
					node = ast_func_call_make(token.attribute.string);
					if (!node)
					{
						print_err_l("Failed to create an AST node in apply_rule()!", ERR_INTERNAL, line);
						return ERR_INTERNAL;
					}

					node->eval_type = entry->data.function.return_type;
				}
			}
			// If it is not defined, it is a call of a not-yet defined function
			else
			{
				// Create the AST node
				node = ast_func_call_make(token.attribute.string);

				if (!node)
				{
					print_err_l("Failed to create an AST node in apply_rule()!", ERR_INTERNAL, line);
					return ERR_INTERNAL;
				}

				node->eval_type = TYPE_NONE;
			}
		}

		result = symstack_pop_multiple(stack, 2);
		if (result)
			return result;
		break;
	// (E)
	case RULE_PARENTHESIS:
		result = symstack_pop(stack);
		if (result)
			return result;

		node = stack->top->node;

		result = symstack_pop_multiple(stack, 3);
		if (result)
			return result;
		break;
	// ()
	case RULE_UNIT:
		literal_t lit;
		lit.literal_type = TYPE_UNIT;
		node = ast_literal_make(&lit);
		if (node == NULL)
		{
			print_err_l("Failed to create a unit literal AST node.", ERR_INTERNAL, line);
			return ERR_INTERNAL;
		}

		result = symstack_pop_multiple(stack, 3);
		if (result)
			return result;
		break;
	// E.E
	case RULE_DOT:
		if (stack->top->node->node_type != AST_FUNC_CALL)
		{
			print_err_l("The \".\" operator must be followed by an extension signature.", ERR_SYNTAX, line);
			return ERR_SYNTAX;
		}

		// Reuse the function call node, change its type to ext_call, and add the base node
		// The unique id and existence of the extension will be resolved during the semantic pass
		node = stack->top->node;
		node->node_type = AST_EXT_CALL;
		node->eval_type = TYPE_NONE;
		node->ext_call.base_node = stack->top->next->next->node;

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// !E
	case RULE_NEG:
		node = ast_unary_op_make(OP_NEG, stack->top->node);

		result = symstack_pop_multiple(stack, 3);
		if (result)
			return result;
		break;
	// +E
	case RULE_UPLUS:
		node = ast_unary_op_make(OP_UPLUS, stack->top->node);

		result = symstack_pop_multiple(stack, 3);
		if (result)
			return result;
		break;
	// -E
	case RULE_UMINUS:
		node = ast_unary_op_make(OP_UMINUS, stack->top->node);

		result = symstack_pop_multiple(stack, 3);
		if (result)
			return result;
		break;
	// E*E
	case RULE_MUL:
		node = ast_binary_op_make(OP_MUL, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E/E
	case RULE_DIV:
		node = ast_binary_op_make(OP_DIV, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E+E
	case RULE_ADD:
		node = ast_binary_op_make(OP_PLUS, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E-E
	case RULE_SUB:
		node = ast_binary_op_make(OP_MINUS, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E<E
	case RULE_LT:
		node = ast_binary_op_make(OP_LESS, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E<=E
	case RULE_LE:
		node = ast_binary_op_make(OP_LESS_EQUAL, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E>E
	case RULE_GT:
		node = ast_binary_op_make(OP_GREATER, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E>=E
	case RULE_GE:
		node = ast_binary_op_make(OP_GREATER_EQUAL, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E==E
	case RULE_EQ:
		node = ast_binary_op_make(OP_EQUAL, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E!=E
	case RULE_NE:
		node = ast_binary_op_make(OP_NOT_EQUAL, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E&&E
	case RULE_AND:
		node = ast_binary_op_make(OP_LOGIC_AND, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E||E
	case RULE_OR:
		node = ast_binary_op_make(OP_LOGIC_OR, stack->top->next->next->node, stack->top->node);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	default:
		break;
	}

	if (node == NULL)
	{
		print_err("Failed to create an AST node.", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	symstack_item_t *item = symstack_create_item();
	if (!item)
	{
		print_err("Failed to create an item inside apply_rule().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	item->type = STACKITEM_TYPE_EXPRESSION;

	item->node = node;
	result = symstack_push(stack, item);
	if (result)
		return result;

	return ERR_OK;
}

/**
 * @brief Checks if an if statement can start at the current position, if so, parses it
 *
 * @param cdata Compiler data
 * @param item Symstack item to be checked
 * @param expect_operand If an operator or operand is currently expected (Prevents the expression consuming another if
 *                       statement)
 * @param stop_at_closing_parenthesis If the expression should stop at a closing bracket
 * @param expression_consumed Returns true if an expression was parsed
 * @return err_t Error code
 */
err_t try_parse_if_expression(compiler_data_t *cdata, symstack_item_t *item, bool expect_operand,
							  bool stop_at_closing_parenthesis, bool *expression_consumed)
{
	if (!cdata || !item || !expression_consumed)
	{
		print_err("Null-reference error in try_parse_if_expression().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	*expression_consumed = false;
	bool stop_at_delimiter = stop_at_closing_parenthesis;

	if (!expect_operand || item->source_token.type != TOKEN_KEYWORD || item->source_token.attribute.keyword != KW_IF)
		return ERR_OK;

	// Consume if
	err_t result = get_next_token(&cdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	// Parse the condition node
	ast_node_t *condition_node = NULL;
	result = parse_expression_internal(cdata, &condition_node, stop_at_delimiter);
	if (result)
	{
		destruct(condition_node);
		return result;
	}

	// Consume then
	if (cdata->token.type != TOKEN_KEYWORD || cdata->token.attribute.keyword != KW_THEN)
	{
		print_err_l("Expected the \"then\" keyword after an if expression condition.", ERR_SYNTAX, cdata->token.line);
		destruct(condition_node);
		return ERR_SYNTAX;
	}

	result = get_next_token(&cdata->token, EOL_OPTIONAL);
	if (result)
	{
		destruct(condition_node);
		return result;
	}

	// Parse the then node
	ast_node_t *then_node = NULL;
	result = parse_expression_internal(cdata, &then_node, stop_at_delimiter);
	if (result)
	{
		destruct(condition_node);
		destruct(then_node);
		return result;
	}

	// Consume else
	if (cdata->token.type != TOKEN_KEYWORD || cdata->token.attribute.keyword != KW_ELSE)
	{
		print_err_l("Expected the \"else\" keyword after an if expression branch.", ERR_SYNTAX, cdata->token.line);
		destruct(condition_node);
		destruct(then_node);
		return ERR_SYNTAX;
	}

	result = get_next_token(&cdata->token, EOL_OPTIONAL);
	if (result)
	{
		destruct(condition_node);
		destruct(then_node);
		return result;
	}

	// Parse the else node
	ast_node_t *else_node = NULL;
	result = parse_expression_internal(cdata, &else_node, stop_at_delimiter);
	if (result)
	{
		destruct(condition_node);
		destruct(then_node);
		destruct(else_node);
		return result;
	}

	// Create the AST node
	item->type = STACKITEM_TYPE_TERMINAL;
	item->data.terminal = TERMINAL_I;
	item->node = ast_if_expr_make(condition_node, then_node, else_node);
	if (!item->node)
	{
		print_err_l("Failed to create an AST node for an if expression.", ERR_INTERNAL, item->source_token.line);
		destruct(condition_node);
		destruct(then_node);
		destruct(else_node);
		return ERR_INTERNAL;
	}

	*expression_consumed = true;
	return ERR_OK;
}

/**
 * @brief Checks if the next item is a function call by peeking the next token. If so, parses the function call.
 *
 * @param cdata Compiler data
 * @param item Symstack item to be checked
 * @param call_consumed If a function call was parsed
 * @return err_t Error code
 */
err_t try_parse_func_call(compiler_data_t *cdata, symstack_item_t *item, bool *call_consumed)
{
	if (!cdata || !item || !call_consumed)
	{
		print_err("Null-reference error in try_parse_func_call().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	// Only identifiers can be the start of function calls
	*call_consumed = false;
	if (item->source_token.type != TOKEN_IDENTIFIER)
		return ERR_OK;

	// Check if the next token is an opening bracket
	token_t next_token;
	err_t result = peek_next_token(&next_token, EOL_OPTIONAL);
	if (result)
		return result;
	if (next_token.type != TOKEN_OPENING_BRACKET || next_token.starts_logical_line)
		return ERR_OK;

	// Parse the function call
	result = parse_function_call(cdata, &item->source_token, &item->node);
	if (result)
		return result;

	*call_consumed = true;
	return ERR_OK;
}

/**
 * @brief Parses a function call, consumes all its tokens and gives back a func_call node
 *
 * @param cdata Compiler data
 * @param identifier_token Identifier of the function
 * @param return_node Pointer to where the AST func_call node should be saved
 * @return err_t Error code
 */
err_t parse_function_call(compiler_data_t *cdata, const token_t *identifier_token, ast_node_t **return_node)
{
	// Find the identifer in the table
	symbol_data_t *entry = symtable_item_find(&cdata->function_table, identifier_token->attribute.string);
	bool func_defined = false;

	if (entry)
	{
		if (entry->symbol_type != SYM_FUNCTION)
			return ERR_INTERNAL;
		func_defined = true;
	}

	// Create the function call node
	ast_node_t *call_node = ast_func_call_make(identifier_token->attribute.string);
	if (!call_node)
	{
		print_err_l("Failed to create a function-call AST node.", ERR_INTERNAL, identifier_token->line);
		return ERR_INTERNAL;
	}

	if (func_defined)
		call_node->eval_type = entry->data.function.return_type;
	else
		call_node->eval_type = TYPE_NONE;

	// Parse the parameter lists
	size_t param_list_count = 0;
	err_t result = get_next_token(&cdata->token, EOL_OPTIONAL);
	if (result)
		return result;

	// Cycle through parameter lists
	while (cdata->token.type == TOKEN_OPENING_BRACKET && !cdata->token.starts_logical_line)
	{
		// Add an empty parameter list into the AST
		result = ast_func_call_add_empty_param_list(call_node);
		if (result)
			return result;

		// Consume (
		result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;

		// Cycle through all parameters inside the parameter list
		if (cdata->token.type != TOKEN_CLOSING_BRACKET)
		{
			size_t param_count = 0;
			while (true)
			{
				// Parse the parameter with the expression parser
				ast_node_t *parameter = NULL;
				result = parse_expression_internal(cdata, &parameter, true);
				if (result)
					return result;

				// Add the parameter into the AST node
				result = ast_func_call_add_param(call_node, parameter, param_list_count);
				if (result)
				{
					return result;
				}
				param_count++;

				// Check for a comma separating the parameters
				if (cdata->token.type == TOKEN_COMMA)
				{
					result = get_next_token(&cdata->token, EOL_OPTIONAL);
					if (result)
						return result;

					// FIXME: allow the trailing comma in certain situations
					if (cdata->token.type == TOKEN_CLOSING_BRACKET)
					{
						print_err_l("Expected a parameter after the comma.", ERR_SYNTAX, cdata->token.line);
						return ERR_SYNTAX;
					}
					continue;
				}

				if (cdata->token.type == TOKEN_CLOSING_BRACKET)
				{
					break;
				}

				print_err_l("Expected a comma or closing parenthesis in a function call.", ERR_SYNTAX,
							cdata->token.line);
				return ERR_SYNTAX;
			}
		}

		param_list_count++;

		// Break out of the parameter list loop
		result = get_next_token(&cdata->token, EOL_OPTIONAL);
		if (result)
			return result;
		if (cdata->token.type != TOKEN_OPENING_BRACKET)
			break;
	}

	*return_node = call_node;
	return ERR_OK;
}

/**
 * @brief Parses an expression
 *
 * @param cdata Compiler data
 * @param return_node Address to which the address of the created AST note should be written
 * @param stop_at_call_delimiter If true, the parser will stop successfuly at a ) if the expression was reduced
 *                               completely and it is not inside parenthesis. If false, this situation will cause an
 *                               error
 *
 * @return err_t Error code
 */
err_t parse_expression_internal(compiler_data_t *cdata, ast_node_t **return_node, bool stop_at_call_delimiter)
{
	symstack_t stack;
	unsigned expression_start_line;
	bool expect_unary = true;
	unsigned parenthesis_level = 0;
	bool expression_started = false;
	bool call_consumed;

	err_t result = symstack_init(&stack);
	if (result)
		return result;

	symstack_item_t *b = symstack_create_item();
	if (!b)
	{
		print_err("Failed to create an item inside parse_expression().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	result = token_to_stack_item(&cdata->token, b, &expect_unary);
	if (result)
		return result;

	result = try_parse_func_call(cdata, b, &call_consumed);
	if (result)
		return result;
	if (!call_consumed)
	{
		result = try_parse_if_expression(cdata, b, expect_unary, stop_at_call_delimiter, &call_consumed);
		if (result)
			return result;
	}

	expression_start_line = b->source_token.line;

	while (!((b->type == STACKITEM_TYPE_TERMINAL && b->data.terminal == TERMINAL_DOLLAR) &&
			 (stack.top->type == STACKITEM_TYPE_EXPRESSION && stack.top->next->type == STACKITEM_TYPE_TERMINAL &&
			  stack.top->next->data.terminal == TERMINAL_DOLLAR)))
	{
		if (stop_at_call_delimiter && cdata->token.type == TOKEN_CLOSING_BRACKET && parenthesis_level == 0 &&
			stack.top->type == STACKITEM_TYPE_EXPRESSION && stack.top->next->type == STACKITEM_TYPE_TERMINAL &&
			stack.top->next->data.terminal == TERMINAL_DOLLAR)
			break;

		// If the new token is the first one on a line, is not the first token of the expression,
		// and is not inside parenthesis, it marks the end of the expression
		if (expression_started && parenthesis_level == 0 && b->source_token.starts_logical_line)
		{
			b->type = STACKITEM_TYPE_TERMINAL;
			b->data.terminal = TERMINAL_DOLLAR;
		}

		const symstack_item_t *a = symstack_top_terminal(&stack);

		switch (prec_table[a->data.terminal][b->data.terminal])
		{
		case E:
			symstack_push(&stack, b);

			// Keep track of parenthesis
			expression_started = true;
			if (b->source_token.type == TOKEN_OPENING_BRACKET)
				parenthesis_level++;
			else if (b->source_token.type == TOKEN_CLOSING_BRACKET && parenthesis_level > 0)
				parenthesis_level--;

			// Only get the next token if it wasn't already requested while parsing a function call
			if (!call_consumed)
			{
				result = get_next_token(&cdata->token, EOL_OPTIONAL);
				if (result)
					return result;
			}

			b = symstack_create_item();
			if (!b)
			{
				print_err("Failed to create an item inside parse_expression().", ERR_INTERNAL);
				return ERR_INTERNAL;
			}
			result = token_to_stack_item(&cdata->token, b, &expect_unary);
			if (result)
				return result;

			// Try to parse a function call, but only if it is not on another logical line
			if (expression_started && parenthesis_level == 0 && b->source_token.starts_logical_line)
				call_consumed = false;
			else
			{
				result = try_parse_func_call(cdata, b, &call_consumed);
				if (result)
					return result;
				if (!call_consumed)
				{
					result = try_parse_if_expression(cdata, b, expect_unary,
													 stop_at_call_delimiter || parenthesis_level > 0, &call_consumed);
					if (result)
						return result;
				}
			}

			break;
		case S:
			symstack_item_t *item = symstack_create_item();
			item->type = STACKITEM_TYPE_OPERATOR;
			item->data.operator = S;
			result = symstack_insert_before(&stack, item, a);
			if (result)
				return result;

			result = symstack_push(&stack, b);
			if (result)
				return result;

			// Keep track of parenthesis
			expression_started = true;
			if (b->source_token.type == TOKEN_OPENING_BRACKET)
				parenthesis_level++;
			else if (b->source_token.type == TOKEN_CLOSING_BRACKET && parenthesis_level > 0)
				parenthesis_level--;

			//  Only get the next token if it wasn't already requested while parsing a function call
			if (!call_consumed)
			{
				result = get_next_token(&cdata->token, EOL_OPTIONAL);
				if (result)
					return result;
			}

			b = symstack_create_item();
			if (!b)
			{
				print_err("Failed to create an item inside parse_expression().", ERR_INTERNAL);
				return ERR_INTERNAL;
			}
			result = token_to_stack_item(&cdata->token, b, &expect_unary);
			if (result)
				return result;

			// Try to parse a function call, but only if it is not on another logical line
			if (expression_started && parenthesis_level == 0 && b->source_token.starts_logical_line)
				call_consumed = false;
			else
			{
				result = try_parse_func_call(cdata, b, &call_consumed);
				if (result)
					return result;
				if (!call_consumed)
				{
					result = try_parse_if_expression(cdata, b, expect_unary,
													 stop_at_call_delimiter || parenthesis_level > 0, &call_consumed);
					if (result)
						return result;
				}
			}

			break;
		case R:
			prec_rule_t rule = find_rule(&stack);

			if (rule == RULE_NONE)
			{
				print_err_l("Unexpected token in an expression.", ERR_SYNTAX, cdata->token.line);
				return ERR_SYNTAX;
			}

			result = apply_rule(rule, &stack, cdata, expression_start_line);
			if (result)
				return result;
			break;
		case N:
			print_err_l("Unexpected token sequence in an expression.", ERR_SYNTAX, cdata->token.line);
			return ERR_SYNTAX;
		}
	}

	*return_node = stack.top->node;

	symstack_destroy(&stack);
	free(b);

	return ERR_OK;
}

err_t parse_expression(compiler_data_t *cdata, ast_node_t **return_node)
{
	return parse_expression_internal(cdata, return_node, false);
}
