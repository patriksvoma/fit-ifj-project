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
	{S, S, R, S, S, S, S, S, S, N, N, R, R, R, R, R, R, R, R, R},
	{S, S, R, S, S, S, S, S, S, N, N, R, R, R, R, R, R, R, R, R},
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
 * @return err_t Error code
 */
err_t token_to_stack_item(const token_t *token, symstack_item_t *stack_item)
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
		break;
	case TOKEN_KEYWORD:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		break;
	case TOKEN_OPENING_BRACKET:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_LEFT_BRACKET;
		break;
	case TOKEN_CLOSING_BRACKET:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_RIGHT_BRACKET;
		break;
	case TOKEN_BOOLEAN:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_I;
		break;
	case TOKEN_INT:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_I;
		break;
	case TOKEN_DOUBLE:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_I;
		break;
	case TOKEN_STRING:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_I;
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
		item.data.terminal = TERMINAL_ADD; // TODO: Figure out if it is a unary or binary plus
		break;
	case TOKEN_MINUS:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_SUB; // TODO: Figure out if it is a unary or binary minus
		break;
	case TOKEN_ASTERISK:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_MUL;
		break;
	case TOKEN_DIV:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DIV;
		break;
	case TOKEN_ASSIGN:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_DOLLAR;
		break;
	case TOKEN_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_EQ;
		break;
	case TOKEN_NOT_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_NE;
		break;
	case TOKEN_GREATER:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_GT;
		break;
	case TOKEN_GREATER_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_GET;
		break;
	case TOKEN_LESS:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_LT;
		break;
	case TOKEN_LESS_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_LET;
		break;
	case TOKEN_AND:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_AND;
		break;
	case TOKEN_OR:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_OR;
		break;
	case TOKEN_NEG:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = TERMINAL_U_NEG;
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

err_t parse_expression(parser_data_t *pdata, token_t *seed_token)
{
	bool use_seed = seed_token;

	// TODO: Remove suppressions
	(void)pdata;
	(void)seed_token;
	(void)use_seed;

	// symstack_init();

	while (true)
	{
		// a = symstack top TERMINAL !;
		// b = symbol z tokenu;

		// switch (prec_table[a][b])
		// {
		// case E:
		// 	symstack_push(b);
		// 	// FIXME: EOL should only be optional if inside parenthesis
		// 	get_next_token(&pdata->token, EOL_OPTIONAL);
		// 	break;
		// case S:
		// 	symstack_push(<);
		// 	symstack_push(b);
		// 	get_next_token(&pdata->token, EOL_OPTIONAL);
		// 	break;
		// case R:
		// 	if symstack_top ()
		// 		== < y &&existuje pravidlo A->y
		// 		{
		// 		}
		// 	else
		// 	{
		// 		chyba
		// 	}
		// 	break;
		// case N:
		// 	chyba break;
		// }
	}
}
