/**
 * @file error.c
 * @brief Error code definitions and handling
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-11
 */

#include "error.h"
#include <stdio.h>

void print_err(const char *msg, err_t error)
{
	fprintf(stderr, "%s ", msg);

	switch (error)
	{
	case ERR_SCANNER:
		fprintf(stderr, "[LEXICAL ERROR]");
		break;
	case ERR_SYNTAX:
		fprintf(stderr, "[SYNTAX ERROR]");
		break;
	case ERR_SEMANTIC_UNDEFINED:
		fprintf(stderr, "[SEMANTIC ERROR - UNDEFINED]");
		break;
	case ERR_SEMANTIC_REDEFINITION:
		fprintf(stderr, "[SEMANTIC ERROR - REDEFINITION]");
		break;
	case ERR_SEMANTIC_INVALID_ARG:
		fprintf(stderr, "[SEMANTIC ERROR - INVALID ARGUMENTS]");
		break;
	case ERR_SEMANTIC_INVALID_TYPE:
		fprintf(stderr, "[SEMANTIC ERROR - INVALID TYPE]");
		break;
	case ERR_SEMANTIC_IMMUTABLE:
		fprintf(stderr, "[SEMANTIC ERROR - IMMUTABLE]");
		break;
	case ERR_SEMANTIC_OTHER:
		fprintf(stderr, "[SEMANTIC ERROR]");
		break;
	case ERR_INTERNAL:
		fprintf(stderr, "[INTERNAL ERROR]");
		break;
	default:
		fprintf(stderr, "[UNKNOWN ERROR]");
		break;
	}

	fprintf(stderr, "\n");
}

void print_err_l(const char *msg, err_t error, unsigned line)
{
	fprintf(stderr, "Line %u: ", line);

	print_err(msg, error);
}
