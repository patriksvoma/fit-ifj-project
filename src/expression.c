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
 * @brief Precedence table
 *
 */
prec_table_sign_t prec_table[PREC_TABLE_SIZE][PREC_TABLE_SIZE] = {
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
err_t token_to_stack_item(token_t *token, symstack_item_t *stack_item)
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
		item.data.terminal = INDEX_I;
		break;
	case TOKEN_KEYWORD:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_DOLLAR;
		break;
	case TOKEN_OPENING_BRACKET:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_LEFT_BRACKET;
		break;
	case TOKEN_CLOSING_BRACKET:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_RIGHT_BRACKET;
		break;
	case TOKEN_BOOLEAN:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_I;
		break;
	case TOKEN_INT:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_I;
		break;
	case TOKEN_DOUBLE:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_I;
		break;
	case TOKEN_STRING:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_I;
		break;
	case TOKEN_EOF:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_DOLLAR;
		break;
	case TOKEN_FOR_GEN_ASSIGN:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_DOLLAR;
		break;
	case TOKEN_DOT:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_DOT;
		break;
	case TOKEN_COMMA:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_DOLLAR;
		break;
	case TOKEN_COLON:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_DOLLAR;
		break;
	case TOKEN_UNDERSCORE:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_DOLLAR;
		break;
	case TOKEN_PLUS:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_ADD; // TODO: Figure out if it is a unary or binary plus
		break;
	case TOKEN_MINUS:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_SUB; // TODO: Figure out if it is a unary or binary minus
		break;
	case TOKEN_ASTERISK:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_MUL;
		break;
	case TOKEN_DIV:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_DIV;
		break;
	case TOKEN_ASSIGN:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_DOLLAR;
		break;
	case TOKEN_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_EQ;
		break;
	case TOKEN_NOT_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_NE;
		break;
	case TOKEN_GREATER:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_GT;
		break;
	case TOKEN_GREATER_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_GET;
		break;
	case TOKEN_LESS:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_LT;
		break;
	case TOKEN_LESS_EQUAL:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_LET;
		break;
	case TOKEN_AND:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_AND;
		break;
	case TOKEN_OR:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_OR;
		break;
	case TOKEN_NEG:
		item.type = STACKITEM_TYPE_TERMINAL;
		item.data.terminal = INDEX_U_NEG;
		break;
	default:
		print_err_l("Invalid token type in expression.", ERR_SYNTAX, token->line);
		return ERR_SYNTAX;
	}

	*stack_item = item;
	return ERR_OK;
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
