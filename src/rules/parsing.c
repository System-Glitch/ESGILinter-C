//Handles all rules which need parsing

#include "rules/parsing.h"
#include "parsing_type.h"
#include "parsing_functions.h"
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
	unsigned int result      = 0;
	scope_t *scope           = NULL;
	char *line               = NULL;
	char *message            = NULL;
	function_t     *function = NULL;
	field_t        *field    = NULL;
	invalid_call_t *call     = NULL;
	messages_t     *messages = malloc(sizeof(messages_t));

	if(messages == NULL) exit(1);

	if(functions == NULL && variables == NULL) {
		messages->functions_list = arraylist_init(root_scope->functions->size);
		messages->variables_list = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
		fill_functions_list(root_scope, messages->functions_list);
		fill_variables_list(root_scope, messages->variables_list);
	}

	for(int i = root_scope->from_line ; i < root_scope->to_line ; i++) {
		scope = get_child_scope(root_scope, i);
		if(scope != root_scope) {
			if(i == scope->from_line) {
				result += parse_and_check(scope, file, variables, functions);
			}
		} else {
			messages->undeclared_functions = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			messages->undeclared_variables = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			messages->invalid_params       = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			messages->invalid_calls        = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);

			line = arraylist_get(file, i);
			type = parse_expression(line, i, scope, messages);
			if(!strcmp(type.name, "NULL")) {
				type = parse_operation(line, i, scope, messages);
			}

			for(size_t j = 0 ; j < messages->undeclared_functions->size ; j++) {
				function = arraylist_get(messages->undeclared_functions, j);
				message = strconcat("Undeclared function: ", function->name);
				print_error("fictive_file.c", i, line, message);
				free(message);
				result++;
			}

			for(size_t j = 0 ; j < messages->undeclared_variables->size ; j++) {
				message = strconcat("Undeclared variable: ", arraylist_get(messages->undeclared_variables, j));
				print_error("fictive_file.c", i, line, message);
				free(message);
				result++;
			}

			for(size_t j = 0 ; j < messages->invalid_calls->size ; j++) {
				call = arraylist_get(messages->invalid_calls, j);
				message = strconcat(call->more > 0 ? "Too many arguments for function: " : "Too few arguments for function: ", call->name);
				print_error("fictive_file.c", i, line, message);
				free(message);
				free(call->name);
				result++;
			}

			for(size_t j = 0 ; j < messages->invalid_params->size ; j++) {
				field = arraylist_get(messages->invalid_params, j);
				message = strconcat("Invalid parameter type: ", field->name);
				print_error("fictive_file.c", i, line, message);
				free(message);
				result++;
			}

			function_list_free(messages->undeclared_functions);
			field_list_free(messages->invalid_params);
			arraylist_free(messages->undeclared_variables, 1);
			arraylist_free(messages->invalid_calls, 1);
			free(type.name);
		}
		
	}

	if(root_scope->parent == NULL) {
		for(size_t j = 0 ; j < messages->variables_list->size ; j++) {
			field = arraylist_get(messages->variables_list, j);
			message = field->is_param ? strconcat("Unused parameter: ", field->name) : strconcat("Unused variable: ", field->name);
			print_warning("fictive_file.c", field->line, arraylist_get(file, field->line), message);
			free(message);
			result++;
		}

		for(size_t j = 0 ; j < messages->functions_list->size ; j++) {
			function = arraylist_get(messages->functions_list, j);
			message = strconcat("Unused function: ", function->name);
			print_warning("fictive_file.c", function->line, arraylist_get(file, function->line), message);
			free(message);
			result++;
		}

		arraylist_free(messages->functions_list, 0);
		arraylist_free(messages->variables_list, 0);

		free(messages);
	}

	return result;
}
