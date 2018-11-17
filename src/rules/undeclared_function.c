#include "rules/undeclared_function.h"
#include "parsing_type.h"
#include "arraylist.h"
#include "display.h"

static scope_t *get_child_scope(scope_t *scope, int line) {
	scope_t *child = is_in_child_scope(scope, line);
	return child == NULL ? scope : child;
}
 
unsigned int check_undeclared_functions(scope_t *root_scope, arraylist_t *file) {

	type_t type;
	unsigned int result = 0;
	scope_t *scope      = root_scope;
	char *line          = NULL;
	char *message       = NULL;
	function_t  *function             = NULL;
	field_t     *field                = NULL;
	arraylist_t *undeclared_functions = NULL;
	arraylist_t *undeclared_variables = NULL;
	arraylist_t *invalid_params       = NULL;

	for(size_t i = 0 ; i < file->size ; i++) {
		
		undeclared_functions = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
		undeclared_variables = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
		invalid_params       = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);

		line = arraylist_get(file, i);
		scope = get_child_scope(scope, i);
		type = get_expression_type(line, i, scope, undeclared_variables, undeclared_functions, invalid_params); //Check type equals
		for(size_t i = 0 ; i < undeclared_functions->size ; i++) {
			function = arraylist_get(undeclared_functions, i);
			message = strconcat("Undeclared function: ", function->name);
			print_error("fictive_file.c", i, line, message);
			free(message);
			result++;
		}

		for(size_t i = 0 ; i < undeclared_variables->size ; i++) {
			message = strconcat("Undeclared variable: ", arraylist_get(undeclared_variables, i));
			print_error("fictive_file.c", i, line, message);
			free(message);
			result++;
		}

		for(size_t i = 0 ; i < invalid_params->size ; i++) {
			field = arraylist_get(invalid_params, i);
			message = strconcat("Invalid parameter type: ", field->name);
			print_error("fictive_file.c", i, line, message);
			free(message);
			result++;
		}

		function_list_free(undeclared_functions);
		field_list_free(invalid_params);
		arraylist_free(undeclared_variables, 1);
	}

	return result;
}