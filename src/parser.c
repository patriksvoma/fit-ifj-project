/**
 * @file parser.c
 * @brief Syntax analyzer for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "parser.h"

/**
 * @brief The entry rule of syntax analysis <prologue>
 *
 * @param pdata
 * @return err_t
 */
err_t prologue(parser_data_t *pdata)
{
	err_t result = get_next_token(&pdata->token);
	if (result)
	{
		return result;
	}
	// TODO:
	// if (token != )

	return ERR_OK;
}

err_t parse()
{
	parser_data_t pdata = {};

	err_t result = prologue(&pdata);

	return result;
}
