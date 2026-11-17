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

#define PREC_TABLE_SIZE 20

/**
 * @brief All possible grammar rules
 *
 */
typedef enum
{
	RULE_NONE,		  // Invalid rule
	RULE_I,			  // E :: i
	RULE_PARENTHESIS, // E :: (E)
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
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, N, N, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, N, N, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, N, N, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, N, N, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, N, N, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, N, N, N, N, N, N, R, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, S, S, S, S, S, S, N, R, R},
	{S, S, R, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, N, R},
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
	// TODO: Maybe copy the string if the token is an identificator

	switch (token->type)
	{
	case TOKEN_IDENTIFIER:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_I;
		*expect_unary = false;
		break;
	case TOKEN_KEYWORD:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
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
	// <(E)
	else if (top1->type == STACKITEM_TYPE_TERMINAL && top1->data.terminal == TERMINAL_RIGHT_BRACKET)
	{
		if (!top3 || !top4)
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
 * @param line Line on which the expression is located
 * @return err_t Error code
 */
err_t apply_rule(prec_rule_t rule, symstack_t *stack, unsigned line)
{
	err_t result;

	data_type_t op1_type = TYPE_NONE;
	data_type_t op2_type = TYPE_NONE;
	ast_node_t *node = NULL;

	switch (rule)
	{
	// i
	case RULE_I:
		token_t token = stack->top->source_token;
		if (token.type == TOKEN_BOOLEAN)
		{
			literal_t lit;
			lit.literal_type = TYPE_BOOLEAN;
			lit.value.boolean = token.attribute.boolean;

			node = ast_add_literal(&lit);
		}
		else if (token.type == TOKEN_INT)
		{
			literal_t lit;
			lit.literal_type = TYPE_INT;
			lit.value.integer = token.attribute.integer;

			node = ast_add_literal(&lit);
		}
		else if (token.type == TOKEN_DOUBLE)
		{
			literal_t lit;
			lit.literal_type = TYPE_DOUBLE;
			lit.value.decimal = token.attribute.decimal;

			node = ast_add_literal(&lit);
		}
		else if (token.type == TOKEN_STRING)
		{
			literal_t lit;
			lit.literal_type = TYPE_STRING;
			lit.value.string = token.attribute.string;

			node = ast_add_literal(&lit);
		}
		else if (token.type == TOKEN_IDENTIFIER)
		{
			// TODO: Check the symbol table, figure out if it is a function or a variable.
			// If it is a visible variable, call add_identifier().
			// If not, make a function call, the existence of the function will be checked
			// before generating output code, as the function can be declared later
			//
			// Probably will need lookahead to be implemented into the scanner to support calls without parameter lists
			node = ast_add_identifier(token.attribute.string, TYPE_INT);
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

		node = stack->top->data.expression_node;

		result = symstack_pop_multiple(stack, 3);
		if (result)
			return result;
		break;
	// E.E
	case RULE_DOT:
		// TODO: Implement
		break;
	// !E
	case RULE_NEG:
		// Type check
		op2_type = stack->top->data.expression_node->eval_type;
		if (op2_type != TYPE_BOOLEAN)
		{
			print_err_l("Invalid operand type for unary operator \"!\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_unary_op(stack->top->data.expression_node, OP_NEG);

		result = symstack_pop_multiple(stack, 3);
		if (result)
			return result;
		break;
	// +E
	case RULE_UPLUS:
		// Type check
		op2_type = stack->top->data.expression_node->eval_type;
		if (op2_type != TYPE_INT && op2_type != TYPE_DOUBLE)
		{
			print_err_l("Invalid operand type for unary operator \"+\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_unary_op(stack->top->data.expression_node, OP_UPLUS);

		result = symstack_pop_multiple(stack, 3);
		if (result)
			return result;
		break;
	// -E
	case RULE_UMINUS:
		// Type check
		op2_type = stack->top->data.expression_node->eval_type;
		if (op2_type != TYPE_INT && op2_type != TYPE_DOUBLE)
		{
			print_err_l("Invalid operand type for unary operator \"-\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_unary_op(stack->top->data.expression_node, OP_UMINUS);

		result = symstack_pop_multiple(stack, 3);
		if (result)
			return result;
		break;
	// E*E
	case RULE_MUL:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE))
		{
			print_err_l("Invalid operand type for binary operator \"*\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node,
								 OP_ASTERISK);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E/E
	case RULE_DIV:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE))
		{
			print_err_l("Invalid operand type for binary operator \"/\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node =
			ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node, OP_DIV);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E+E
	case RULE_ADD:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE) &&
				 !(op1_type == TYPE_STRING && op2_type == TYPE_STRING))
		{
			print_err_l("Invalid operand type for binary operator \"+\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node =
			ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node, OP_PLUS);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E-E
	case RULE_SUB:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE))
		{
			print_err_l("Invalid operand type for binary operator \"-\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node =
			ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node, OP_MINUS);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E<E
	case RULE_LT:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE) &&
				 !(op1_type == TYPE_STRING && op2_type == TYPE_STRING))
		{
			print_err_l("Invalid operand type for binary operator \"<\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node =
			ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node, OP_LESS);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E<=E
	case RULE_LE:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE) &&
				 !(op1_type == TYPE_STRING && op2_type == TYPE_STRING))
		{
			print_err_l("Invalid operand type for binary operator \"<=\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node,
								 OP_LESS_EQUAL);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E>E
	case RULE_GT:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE) &&
				 !(op1_type == TYPE_STRING && op2_type == TYPE_STRING))
		{
			print_err_l("Invalid operand type for binary operator \">\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node,
								 OP_GREATER);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E>=E
	case RULE_GE:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE) &&
				 !(op1_type == TYPE_STRING && op2_type == TYPE_STRING))
		{
			print_err_l("Invalid operand type for binary operator \">=\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node,
								 OP_GREATER_EQUAL);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E==E
	case RULE_EQ:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE) &&
				 !(op1_type == TYPE_STRING && op2_type == TYPE_STRING) &&
				 !(op1_type == TYPE_BOOLEAN && op2_type == TYPE_BOOLEAN) &&
				 !(op1_type == TYPE_UNIT && op2_type == TYPE_UNIT))
		{
			print_err_l("Invalid operand type for binary operator \"==\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node =
			ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node, OP_EQUAL);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E!=E
	case RULE_NE:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (op1_type == TYPE_INT && op2_type == TYPE_DOUBLE)
		{
			// TODO: Create a int2double node for operand 1
		}
		else if (op1_type == TYPE_DOUBLE && op2_type == TYPE_INT)
		{
			// TODO: Create a int2double node for operand 2
		}
		else if (!(op1_type == TYPE_INT && op2_type == TYPE_INT) &&
				 !(op1_type == TYPE_DOUBLE && op2_type == TYPE_DOUBLE) &&
				 !(op1_type == TYPE_STRING && op2_type == TYPE_STRING) &&
				 !(op1_type == TYPE_BOOLEAN && op2_type == TYPE_BOOLEAN) &&
				 !(op1_type == TYPE_UNIT && op2_type == TYPE_UNIT))
		{
			print_err_l("Invalid operand type for binary operator \"!=\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node,
								 OP_NOT_EQUAL);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E&&E
	case RULE_AND:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (!(op1_type == TYPE_BOOLEAN && op2_type == TYPE_BOOLEAN))
		{
			print_err_l("Invalid operand type for binary operator \"&&\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node,
								 OP_LOGIC_AND);

		result = symstack_pop_multiple(stack, 4);
		if (result)
			return result;
		break;
	// E||E
	case RULE_OR:
		// Type check
		op1_type = stack->top->next->next->data.expression_node->eval_type;
		op2_type = stack->top->data.expression_node->eval_type;

		if (!(op1_type == TYPE_BOOLEAN && op2_type == TYPE_BOOLEAN))
		{
			print_err_l("Invalid operand type for binary operator \"||\".", ERR_SEMANTIC_INVALID_TYPE, line);
			return ERR_SEMANTIC_INVALID_TYPE;
		}

		node = ast_add_binary_op(stack->top->next->next->data.expression_node, stack->top->data.expression_node,
								 OP_LOGIC_OR);

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

	item->data.expression_node = node;
	result = symstack_push(stack, item);
	if (result)
		return result;

	return ERR_OK;
}

err_t parse_expression(parser_data_t *pdata, token_t *seed_token, ast_node_t **return_node)
{
	bool use_seed = seed_token;

	symstack_t stack;
	unsigned expression_start_line;
	bool expect_unary = true;

	err_t result = symstack_init(&stack);
	if (result)
		return result;

	symstack_item_t *b = symstack_create_item();
	if (!b)
	{
		print_err("Failed to create an item inside parse_expression().", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	if (use_seed)
	{
		result = token_to_stack_item(seed_token, b, &expect_unary);
		expression_start_line = seed_token->line;
	}
	else
	{
		result = token_to_stack_item(&pdata->token, b, &expect_unary);
		expression_start_line = pdata->token.line;
	}
	if (result)
		return result;

	while (!((b->type == STACKITEM_TYPE_TERMINAL && b->data.terminal == TERMINAL_DOLLAR) &&
			 (stack.top->type == STACKITEM_TYPE_EXPRESSION && stack.top->next->type == STACKITEM_TYPE_TERMINAL &&
			  stack.top->next->data.terminal == TERMINAL_DOLLAR)))
	{
		const symstack_item_t *a = symstack_top_terminal(&stack);

		switch (prec_table[a->data.terminal][b->data.terminal])
		{
		case E:
			symstack_push(&stack, b);

			// FIXME: EOL should only be optional if inside parenthesis
			if (use_seed)
				use_seed = false;
			else
				result = get_next_token(&pdata->token, EOL_OPTIONAL);
			if (result)
				return result;

			b = symstack_create_item();
			result = token_to_stack_item(&pdata->token, b, &expect_unary);
			if (result)
				return result;

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

			// FIXME: EOL should only be optional if inside parenthesis
			if (use_seed)
				use_seed = false;
			else
				result = get_next_token(&pdata->token, EOL_OPTIONAL);
			if (result)
				return result;

			b = symstack_create_item();
			result = token_to_stack_item(&pdata->token, b, &expect_unary);
			if (result)
				return result;

			break;
		case R:
			prec_rule_t rule = find_rule(&stack);

			if (rule == RULE_NONE)
			{
				print_err_l("Unexpected token in an expression.", ERR_SYNTAX, pdata->token.line);
				return ERR_SYNTAX;
			}

			result = apply_rule(rule, &stack, expression_start_line);
			if (result)
				return result;
			break;
		case N:
			print_err_l("Unexpected token sequence in an expression.", ERR_SYNTAX, pdata->token.line);
			return ERR_SYNTAX;
		}
	}
  
  *return_node = stack.top->data.expression_node;
  
  symstack_destroy(&stack);
	free(b);
  
	return ERR_OK;
}
