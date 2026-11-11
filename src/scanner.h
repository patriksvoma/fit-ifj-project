/**
 * @file scanner.h
 * @brief Scanner module for the IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#pragma once

#include "error.h"
#include <stdbool.h>

/**
 * @brief IFJ26 keyword definitions
 *
 */
typedef enum
{
	KW_DEF,
	KW_DO,
	KW_DOUBLE,
	KW_ELSE,
	KW_IF,
	KW_IMPORT,
	KW_INT,
	KW_MAIN,
	KW_RETURN,
	KW_STRING,
	KW_THEN,
	KW_UNIT,
	KW_VAL,
	KW_VAR,
	KW_WHILE,
	KW_BOOLEAN,
	KW_EXTENSION,
	KW_FOR,
} keyword_t;

/**
 * @brief Token types
 *
 */
typedef enum
{
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD,
	TOKEN_OPENING_BRACKET,
	TOKEN_CLOSING_BRACKET,
	TOKEN_INT,
	TOKEN_DOUBLE,
	TOKEN_STRING,
	TOKEN_MULTLINE_STRING,
	TOKEN_EOF,
	TOKEN_DOT,
	TOKEN_COLON,
	TOKEN_UNDERSCORE,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_ASTERISK,
	TOKEN_DIV,
	TOKEN_ASSIGN,
	TOKEN_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_GREATER,
	TOKEN_GREATER_EQUAL,
	TOKEN_LESS,
	TOKEN_LESS_EQUAL
} token_type_t; // TODO: Figure out all correct token types

/**
 * @brief Optional attributes of an accepted token
 *
 */
typedef struct
{
	keyword_t keyword;
	int integer;
	double decimal;
	char *string;
} token_attr_t;

/**
 * @brief Token structure
 *
 */
typedef struct
{
	token_type_t type;
	token_attr_t attribute;
	unsigned line;
	bool starts_logical_line;
	unsigned indent;
} token_t;

/**
 * @brief Rules for checking newlines before a token
 *
 */
typedef enum
{
	EOL_REQUIRED,
	EOL_OPTIONAL,
	EOL_FORBIDDEN
} eol_rule_t;

/**
 * @brief Get the next token object
 *
 * @param token Pointer to where the accepted token should be saved
 * @param eol Tells the scanner if a newline is required, optional, or forbidden before the next token
 * @return err_t Error
 */
err_t get_next_token(token_t *token, eol_rule_t eol);
