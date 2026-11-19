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
	ast_node_t *ast_root = ast_program_make();

	int err_code = parse(ast_root);

	return err_code;
}
