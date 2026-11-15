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
