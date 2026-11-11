/**
 * @file error.h
 * @brief Error code definitions and handling
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#pragma once

/**
 * @brief Error types and their corresponding return codes as defined in the specificaton
 *
 */
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

/**
 * @brief Prints an error message to stderr in the "msg [error type]" format
 *
 * @param msg Error message
 * @param error Error code, which will be appended to the error message
 */
void print_err(const char *msg, err_t error);

/**
 * @brief Prints an error message to stderr in the "Line 1: msg [error type]" format
 *
 * @param msg Error message
 * @param error Error code, which will be appended to the error message
 * @param line Line on which the error occured
 */
void print_err_l(const char *msg, err_t error, unsigned line);
