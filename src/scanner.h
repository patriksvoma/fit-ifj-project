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
	KW_NONE,	  // Default value
	KW_DEF,		  // def
	KW_DO,		  // do
	KW_DOUBLE,	  // Double
	KW_ELSE,	  // else
	KW_IF,		  // if
	KW_IMPORT,	  // import
	KW_INT,		  // Int
	KW_MAIN,	  // @main
	KW_RETURN,	  // return
	KW_STRING,	  // String
	KW_THEN,	  // then
	KW_UNIT,	  // Unit
	KW_VAL,		  // val
	KW_VAR,		  // var
	KW_WHILE,	  // while
	KW_BOOLEAN,	  // Boolean
	KW_EXTENSION, // extension
	KW_FOR,		  // for
} keyword_t;

/**
 * @brief Token types
 *
 */
typedef enum
{
	TOKEN_NONE,			   // Default value
	TOKEN_IDENTIFIER,	   // Any identifier
	TOKEN_KEYWORD,		   // Any keyword
	TOKEN_OPENING_BRACKET, // (
	TOKEN_CLOSING_BRACKET, // )
	TOKEN_BOOLEAN,		   // Any boolean literal
	TOKEN_INT,			   // Any integer literal
	TOKEN_DOUBLE,		   // Any double literal
	TOKEN_STRING,		   // Any string literal
	TOKEN_EOF,			   // End-of-file
	TOKEN_FOR_GEN_ASSIGN,  // <-
	TOKEN_DOT,			   // .
	TOKEN_COMMA,		   // ,
	TOKEN_COLON,		   // :
	TOKEN_UNDERSCORE,	   // _
	TOKEN_PLUS,			   // +
	TOKEN_MINUS,		   // -
	TOKEN_ASTERISK,		   // *
	TOKEN_DIV,			   // /
	TOKEN_ASSIGN,		   // =
	TOKEN_EQUAL,		   // ==
	TOKEN_NOT_EQUAL,	   // !=
	TOKEN_GREATER,		   // >
	TOKEN_GREATER_EQUAL,   // >=
	TOKEN_LESS,			   // <
	TOKEN_LESS_EQUAL,	   // <=
	TOKEN_AND,			   // &&
	TOKEN_OR,			   // ||
	TOKEN_NEG			   // !
	TOKEN_SEMICOLON		   // ;
} token_type_t;

/**
 * @brief Optional attributes of an accepted token
 *
 */
typedef struct
{
	keyword_t keyword;
	bool boolean;
	long long integer;
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
