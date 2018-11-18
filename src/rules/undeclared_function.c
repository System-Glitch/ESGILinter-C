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
	scope_t *scope      = NULL;
	char *line          = NULL;
	char *message       = NULL;
	function_t  *function             = NULL;
	field_t     *field                = NULL;
	arraylist_t *undeclared_functions = NULL;
	arraylist_t *undeclared_variables = NULL;
	arraylist_t *invalid_params       = NULL;

	for(int i = root_scope->from_line ; i < root_scope->to_line ; i++) {
		scope = get_child_scope(root_scope, i); //segfault?
		if(scope != root_scope) {
			if(i == scope->from_line) {
				result += check_undeclared_functions(scope, file);
			}
		} else {
			undeclared_functions = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			undeclared_variables = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			invalid_params       = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);

			line = arraylist_get(file, i);
			type = get_expression_type(line, i, scope, undeclared_variables, undeclared_functions, invalid_params); //Check type equals
			for(size_t j = 0 ; j < undeclared_functions->size ; j++) {
				function = arraylist_get(undeclared_functions, j);
				message = strconcat("Undeclared function: ", function->name);
				print_error("fictive_file.c", j, line, message);
				free(message);
				result++;
			}

			for(size_t j = 0 ; j < undeclared_variables->size ; j++) {
				message = strconcat("Undeclared variable: ", arraylist_get(undeclared_variables, j));
				print_error("fictive_file.c", j, line, message);
				free(message);
				result++;
			}

			for(size_t j = 0 ; j < invalid_params->size ; j++) {
				field = arraylist_get(invalid_params, j);
				message = strconcat("Invalid parameter type: ", field->name);
				print_error("fictive_file.c", j, line, message);
				free(message);
				result++;
			}

			function_list_free(undeclared_functions);
			field_list_free(invalid_params);
			arraylist_free(undeclared_variables, 1);
			free(type.name);
		}
		
	}

	return result;
}