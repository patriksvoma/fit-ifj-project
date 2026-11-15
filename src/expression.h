/**
 * @file expression.h
 * @brief Operator-Precedence part of the syntax analyzer in the IFJ26 compiler, used for parsing expressions
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-14
 */

#pragma once

#include "error.h"
#include "parser.h"

/**
 * @brief Parses an expression
 *
 * @param pdata Parser data
 * @param seed_token If not NULL, the seed_token is used as the first token in the expression
 * @return err_t Error code
 */
err_t parse_expression(parser_data_t *pdata, token_t *seed_token);
