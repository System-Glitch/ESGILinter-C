//Handles all rules which need parsing

#include "rules/parsing.h"
#include "parsing_type.h"
#include "parsing_expressions.h"
#include "parsing_operations.h"
#include "arraylist.h"
#include "display.h"

static scope_t *get_child_scope(scope_t *scope, int line) {
	scope_t *child = is_in_child_scope(scope, line);
	return child == NULL ? scope : child;
}

static void fill_functions_list(scope_t *root_scope, arraylist_t *functions) {
	function_t *function = NULL;
	for(size_t i = 0 ; i < root_scope->functions->size ; i++) {
		function = arraylist_get(root_scope->functions, i);
		if(!(function->is_prototype)) {
			arraylist_add(functions, function);
		}
	}
}

static void fill_variables_list(scope_t *scope, arraylist_t *variables) {
	node_t * current = scope->children->head;
	
	for(size_t i = 0 ; i < scope->variables->size ; i++) {
		arraylist_add(variables, arraylist_get(scope->variables, i));
	}

	if(current != NULL) {
		do {
			fill_variables_list((scope_t*)current->val, variables);
		} while ((current = current->next) != NULL);
	}
}

unsigned int parse_and_check(scope_t *root_scope, arraylist_t *file, arraylist_t *variables, arraylist_t *functions) {

	type_t type;
	unsigned int result               = 0;
	scope_t *scope                    = NULL;
	char *line                        = NULL;
	char *message                     = NULL;
	function_t  *function             = NULL;
	field_t     *field                = NULL;
	arraylist_t *undeclared_functions = NULL;
	arraylist_t *undeclared_variables = NULL;
	arraylist_t *invalid_params       = NULL;

	if(functions == NULL && variables == NULL) {
		functions = arraylist_init(root_scope->functions->size); //Function is removed when used
		variables = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY); //Variable is removed when used
		fill_functions_list(root_scope, functions);
		fill_variables_list(root_scope, variables);
	}

	for(int i = root_scope->from_line ; i < root_scope->to_line ; i++) {
		scope = get_child_scope(root_scope, i);
		if(scope != root_scope) {
			if(i == scope->from_line) {
				result += parse_and_check(scope, file, variables, functions);
			}
		} else {
			undeclared_functions = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			undeclared_variables = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			invalid_params       = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);

			line = arraylist_get(file, i);
			type = parse_expression(line, i, scope, undeclared_variables, undeclared_functions, invalid_params, variables, functions);
			if(!strcmp(type.name, "NULL")) {
				type = parse_operation(line, i, scope, undeclared_variables, undeclared_functions, invalid_params, variables, functions);
			}

			for(size_t j = 0 ; j < undeclared_functions->size ; j++) {
				function = arraylist_get(undeclared_functions, j);
				message = strconcat("Undeclared function: ", function->name);
				print_error("fictive_file.c", i, line, message);
				free(message);
				result++;
			}

			for(size_t j = 0 ; j < undeclared_variables->size ; j++) {
				message = strconcat("Undeclared variable: ", arraylist_get(undeclared_variables, j));
				print_error("fictive_file.c", i, line, message);
				free(message);
				result++;
			}

			for(size_t j = 0 ; j < invalid_params->size ; j++) {
				field = arraylist_get(invalid_params, j);
				message = strconcat("Invalid parameter type: ", field->name);
				print_error("fictive_file.c", i, line, message);
				free(message);
				result++;
			}

			function_list_free(undeclared_functions);
			field_list_free(invalid_params);
			arraylist_free(undeclared_variables, 1);
			free(type.name);
		}
		
	}

	if(root_scope->parent == NULL) {
		for(size_t j = 0 ; j < variables->size ; j++) {
			field = arraylist_get(variables, j);
			message = field->is_param ? strconcat("Unused parameter: ", field->name) : strconcat("Unused variable: ", field->name);
			print_warning("fictive_file.c", field->line, arraylist_get(file, field->line), message);
			free(message);
			result++;
		}

		for(size_t j = 0 ; j < functions->size ; j++) {
			function = arraylist_get(functions, j);
			message = strconcat("Unused function: ", function->name);
			print_warning("fictive_file.c", function->line, arraylist_get(file, function->line), message);
			free(message);
			result++;
		}

		arraylist_free(functions, 0);
		arraylist_free(variables, 0);
	}

	return result;
}
