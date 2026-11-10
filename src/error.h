/**
 * @file error.h
 * @brief Error code definitions
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#pragma once

typedef enum
{
	ERR_OK = 0,
	ERR_SCANNER = 1,
	ERR_SYNTAX = 2,
	ERR_SEMANTIC_UNDEFINED = 3,
	ERR_SEMANTIC_REDEFINITION = 4,
	ERR_SEMANTIC_INVALID_ARG = 5,
	ERR_SEMANTIC_INVALID_TYPE = 6,
	ERR_SEMANTIC_IMMUTABLE = 7,
	ERR_SEMANTIC_OTHER = 10,
	ERR_INTERNAL = 99
} err_t;
