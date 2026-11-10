/**
 * @file main.c
 * @brief The IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "parser.h"

int main()
{
	int err_code = parse();

	return err_code;
}
