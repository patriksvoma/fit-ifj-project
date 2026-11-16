/**
 * @file symtable.h
 * @brief Symbol table for the IFJ26 compiler
 *
 * @author Jakub Kvasnička (xkvasnj00@stud.fit.vut.cz)
 *
 * @date 2026-11-13
 */

#pragma once

#include <stdbool.h>

typedef enum
{
	SYMBOL_VARIABLE,
	SYMBOL_FUNCTION,
}	symbol_type_t;

typedef struct symbol
{
	symbol_type_t type;
	bool defined;
	int params;

	// additional data
} symbol_data_t;

// TODO: symbol tree (key, data, left, right)
