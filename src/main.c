/**
 * @file main.c
 * @brief The IFJ26 compiler
 *
 * @author Patrik Švoma (xsvomap00@stud.fit.vut.cz)
 * @author Petr Marek (xmarekp00@stud.fit.vut.cz)
 *
 * @date 2026-11-10
 */

#include "codegen.h"
#include "parser.h"

int main()
{
	ast_node_t *ast_root = ast_program_make();

	parser_data_t pdata = {};

	int err_code = parse(ast_root, pdata);

	generate_code(ast_root, &pdata);

	return err_code;
}
