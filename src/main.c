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
	if (!ast_root)
	{
		print_err("Failed to create the root AST node!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	parser_data_t pdata = {};

	err_t err_code = parse(ast_root, &pdata);
	if (err_code)
	{
		destruct(ast_root);
		return err_code;
	}

	err_code = generate_code(ast_root, &pdata);

	destruct(ast_root);
	return err_code;
}
