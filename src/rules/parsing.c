//Handles all rules which need parsing

#include "rules/parsing.h"
#include "parsing_type.h"
#include "parsing_functions.h"
#include "parsing_expressions.h"
#include "parsing_operations.h"
#include "arraylist.h"
#include "display.h"
#include "fileloader.h"
#include "conf.h"

static scope_t *get_child_scope(scope_t *scope, int line) {
	scope_t *child = is_in_child_scope(scope, line);
	return child == NULL ? scope : child;
}

static void fill_functions_list(scope_t *root_scope, arraylist_t *functions) {
	function_t *function = NULL;
	for(size_t i = 0 ; i < root_scope->functions->size ; i++) {
		function = arraylist_get(root_scope->functions, i);
		if(!(function->is_prototype) && strcmp(function->name, "sizeof")) {
			arraylist_add(functions, function);
		}
	}
}

static void fill_variables_list(scope_t *scope, arraylist_t *variables) {
	node_t  *current = scope->children->head;
	field_t *field   = NULL;

	for(size_t i = 0 ; i < scope->variables->size ; i++) {
		field = arraylist_get(scope->variables, i);
		arraylist_add(variables, field);
	}

	if(current != NULL) {
		do {
			fill_variables_list((scope_t*)current->val, variables);
		} while ((current = current->next) != NULL);
	}
}

static char *build_wrong_type_message(wrong_type_t *wrong_type, unsigned char message_type) {
	static const char* message_types[] = {
		"Invalid assignment: expected ",
		"Invalid return type: expected ",
		"Ternary condition operands must have the same type: left is "
	};
	char *message = NULL;
	char *tmp = NULL;
	char *seq = NULL;

	message = strconcat((char*)message_types[message_type] , wrong_type->expected_type->name);
	seq = generate_char_sequence('*', wrong_type->expected_type->is_pointer);
	tmp = strconcat(message, seq);
	free(seq);
	free(message);

	message = strconcat(tmp, (char*)(message_type == WRONG_TYPE_TERNARY ? ", right is " : ", actual "));
	free(tmp);
	tmp = strconcat(message, wrong_type->actual_type->name);
	free(message);
	seq = generate_char_sequence('*', wrong_type->actual_type->is_pointer);
	message = strconcat(tmp, seq);
	free(seq);
	free(tmp);

	return message;
}

static void free_wrong_type_list(arraylist_t *list) {
	wrong_type_t *wrong_type = NULL;

	for(size_t i = 0 ; i < list->size ; i++) {
		wrong_type = arraylist_get(list, i);
		free(wrong_type->expected_type->name);
		free(wrong_type->expected_type);
		free(wrong_type->actual_type->name);
		free(wrong_type->actual_type);
	}

	arraylist_free(list, 1);
}

unsigned int parse_and_check(scope_t *root_scope, arraylist_t *file, arraylist_t *functions, arraylist_t *variables, arraylist_t *conf, arraylist_t *real_file) {

	type_t type;
	unsigned int result        = 0;
	unsigned int breaks        = 0;
	scope_t *scope             = NULL;
	char *line                 = NULL;
	char *message              = NULL;
	char *display              = NULL;
	function_t     *function   = NULL;
	field_t        *field      = NULL;
	invalid_call_t *call       = NULL;
	wrong_type_t   *wrong_type = NULL;
	line_t         *line_info  = NULL;
	messages_t     *messages   = malloc(sizeof(messages_t));

	if(messages == NULL) exit(1);

	if(root_scope->parent == NULL) {
		messages->functions_list = arraylist_init(root_scope->functions->size);
		messages->variables_list = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
		fill_functions_list(root_scope, messages->functions_list);
		fill_variables_list(root_scope, messages->variables_list);
	} else {
		messages->functions_list = functions;
		messages->variables_list = variables;
	}

	for(int i = root_scope->from_line ; i <= root_scope->to_line ; i++) {
		scope = get_child_scope(root_scope, i);
		if(scope != root_scope) {
			if(i == scope->from_line) {
				result += parse_and_check(scope, file, messages->functions_list, messages->variables_list, conf, real_file);
			}
		} else {
			messages->undeclared_functions = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			messages->undeclared_variables = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			messages->invalid_params       = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			messages->invalid_calls        = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			messages->wrong_assignment     = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			messages->wrong_return         = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
			messages->ternary_types        = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);

			line_info = get_line(file, i);
			line = line_info->line;
			breaks = strcount(line, '\n') + (line_info->start_real_line == 0 ? 1 : 0);
			type = parse_expression(line, i, scope, messages);
			if(!strcmp(type.name, "NULL")) {
				type = parse_operation(line, i, scope, messages);
			}

			if(conf == NULL || check_rule(conf, "undeclared-function")) {
				for(size_t j = 0 ; j < messages->undeclared_functions->size ; j++) {
					function = arraylist_get(messages->undeclared_functions, j);
					message = strconcat("Undeclared function: ", function->name);
					display = trim(arraylist_get(real_file, line_info->start_real_line + breaks - 1));
					print_error(line_info->source, line_info->start_real_line + breaks, display, message);
					free(display);
					free(message);
					result++;
				}
			}

			if(conf == NULL || check_rule(conf, "undeclared-variable")) {
				for(size_t j = 0 ; j < messages->undeclared_variables->size ; j++) {
					message = strconcat("Undeclared variable: ", arraylist_get(messages->undeclared_variables, j));
					display = trim(arraylist_get(real_file, line_info->start_real_line + breaks - 1));
					print_error(line_info->source, line_info->start_real_line + breaks, display, message);
					free(display);
					free(message);
					result++;
				}
			}

			if(conf == NULL || check_rule(conf, "arguments-count")) {
				for(size_t j = 0 ; j < messages->invalid_calls->size ; j++) {
					call = arraylist_get(messages->invalid_calls, j);
					message = strconcat(call->more > 0 ? "Too many arguments for function: " : "Too few arguments for function: ", call->name);
					display = trim(arraylist_get(real_file, line_info->start_real_line + breaks - 1));
					print_error(line_info->source, line_info->start_real_line + breaks, display, message);
					free(display);
					free(message);
					free(call->name);
					result++;
				}
			}

			if(conf == NULL || check_rule(conf, "function-parameters-type")) {
				for(size_t j = 0 ; j < messages->invalid_params->size ; j++) {
					field = arraylist_get(messages->invalid_params, j);
					message = strconcat("Invalid parameter type: ", field->name);
					display = trim(arraylist_get(real_file, line_info->start_real_line + breaks - 1));
					print_error(line_info->source, line_info->start_real_line + breaks, display, message);
					free(display);
					free(message);
					result++;
				}
			}

			if(conf == NULL || check_rule(conf, "variable-assignment-type")) {
				for(size_t j = 0 ; j < messages->wrong_assignment->size ; j++) {
					wrong_type = arraylist_get(messages->wrong_assignment, j);
					message = build_wrong_type_message(wrong_type, WRONG_TYPE_ASSIGNMENT);
					display = trim(arraylist_get(real_file, line_info->start_real_line + breaks - 1));
					print_error(line_info->source, line_info->start_real_line + breaks, display, message);
					free(display);
					free(message);
					result++;
				}
			}

			if(conf == NULL || check_rule(conf, "return-type")) {
				for(size_t j = 0 ; j < messages->wrong_return->size ; j++) {
					wrong_type = arraylist_get(messages->wrong_return, j);
					message = build_wrong_type_message(wrong_type, WRONG_TYPE_RETURN);
					display = trim(arraylist_get(real_file, line_info->start_real_line + breaks - 1));
					print_error(line_info->source, line_info->start_real_line + breaks, display, message);
					free(display);
					free(message);
					result++;
				}
			}

			if(conf == NULL || check_rule(conf, "ternary-types")) {
				for(size_t j = 0 ; j < messages->ternary_types->size ; j++) {
					wrong_type = arraylist_get(messages->ternary_types, j);
					message = build_wrong_type_message(wrong_type, WRONG_TYPE_TERNARY);
					display = trim(arraylist_get(real_file, line_info->start_real_line + breaks - 1));
					print_error(line_info->source, line_info->start_real_line + breaks, display, message);
					free(display);
					free(message);
					result++;
				}
			}

			function_list_free(messages->undeclared_functions);
			field_list_free(messages->invalid_params);
			arraylist_free(messages->undeclared_variables, 1);
			arraylist_free(messages->invalid_calls, 1);
			arraylist_free(messages->wrong_assignment, 1);
			free_wrong_type_list(messages->wrong_return);
			free_wrong_type_list(messages->ternary_types);
			free(type.name);
		}
		
	}

	if(root_scope->parent == NULL) {

		if(conf == NULL || check_rule(conf, "unused-variable")) {
			for(size_t j = 0 ; j < messages->variables_list->size ; j++) {
				field = arraylist_get(messages->variables_list, j);
				line_info = get_line(file, field->line);
				message = field->is_param ? strconcat("Unused parameter: ", field->name) : strconcat("Unused variable: ", field->name);
				breaks = strcount(line_info->line, '\n') + (line_info->start_real_line == 0 ? 1 : 0);
				display = trim(arraylist_get(real_file, line_info->start_real_line + breaks - 1));
				print_warning(line_info->source, line_info->start_real_line + breaks, display, message);
				free(display);
				free(message);
				result++;
			}
		}

		if(conf == NULL || check_rule(conf, "unused-function")) {
			for(size_t j = 0 ; j < messages->functions_list->size ; j++) {
				function = arraylist_get(messages->functions_list, j);
				line_info = get_line(file, function->line);
				message = strconcat("Unused function: ", function->name);
				breaks = strcount(line_info->line, '\n') + (line_info->start_real_line == 0 ? 1 : 0);
				display = trim(arraylist_get(real_file, line_info->start_real_line + breaks - 1));
				print_warning(line_info->source, line_info->start_real_line + breaks, display, message);
				free(display);
				free(message);
				result++;
			}
		}

		arraylist_free(messages->functions_list, 0);
		arraylist_free(messages->variables_list, 0);

		free(messages);
	}

	return result;
}
