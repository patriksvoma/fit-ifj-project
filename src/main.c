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
#include "semantics.h"

int main()
{
	ast_node_t *ast_root = ast_program_make();
	if (!ast_root)
	{
		print_err("Failed to create the root AST node!", ERR_INTERNAL);
		return ERR_INTERNAL;
	}

	compiler_data_t cdata = {};

	err_t err_code = parse(ast_root, &cdata);
	if (err_code)
	{
		destruct(ast_root);
		local_tables_destroy(&cdata);
		return err_code;
	}

	// Run additional semantic checks
	err_code = do_semantic_checks(ast_root, &cdata);
	if (err_code)
	{
		destruct(ast_root);
		local_tables_destroy(&cdata);
		return err_code;
	}

	// Destroy the local symbol tables
	err_code = local_tables_destroy(&cdata);
	if (err_code)
		return err_code;

	err_code = generate_code(ast_root, &cdata);

	destruct(ast_root);
	return err_code;
}
