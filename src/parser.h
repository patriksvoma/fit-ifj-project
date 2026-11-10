/**
 * @file parser.h
 * @brief Syntax analyzer for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#pragma once

#include "error.h"
#include "scanner.h"

/**
 * @brief Struct for holding the parser data
 *
 */
typedef struct
{
	token_t token;
} parser_data_t;

/**
 * @brief Main parsing function
 *
 * @return err_t Error code
 */
err_t parse();
