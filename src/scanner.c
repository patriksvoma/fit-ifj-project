/**
 * @file scanner.h
 * @brief Scanner module for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "scanner.h"

err_t get_next_token(token_t *token, eol_rule_t eol)
{
	// TODO: This placeholder code is here just to pass code checks
	token->attribute.integer = 0;
	(void)eol;

	return ERR_SCANNER;
}
