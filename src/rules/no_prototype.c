#include <string.h>
#include "rules/no_prototype.h"
#include "arraylist.h"
#include "fileloader.h"
#include "display.h"

static char check_params(function_t *function, function_t *prototype) {
	if(function->params->size != prototype->params->size) return 0;
	for(size_t i = 0 ; i < function->params->size ; i++) {
		if(!type_equals(&((field_t*)arraylist_get(function->params, i))->type, &((field_t*)arraylist_get(prototype->params, i))->type))
			return 0;
	}
	return 1;
}

unsigned int check_no_prototype(scope_t *root_scope, arraylist_t *file) {

	arraylist_t *functions = root_scope->functions;
	function_t *function   = NULL;
	function_t *prototype  = NULL;
	line_t     *line       = NULL;
	unsigned int count     = 0; //Counts warnings and errors

	for(size_t i = 0 ; i < functions->size ; i++) {
		function = arraylist_get(functions, i);
		if(!function->is_prototype && strcmp(function->name, "main")) {

			prototype = find_function_prototype(root_scope, function->name);
			if(prototype != NULL) {
				if(strcmp(function->return_type.name, prototype->return_type.name) || !check_params(function, prototype)) {
					line = get_line(file, prototype->line);
					print_error(line->source, line->start_real_line, trim_heading_whitespaces(line->line), "Conflicting types");
					count++;
				}
			} else {
				line = get_line(file, function->line);
				print_warning(line->source, line->start_real_line, trim_heading_whitespaces(line->line), "Missing prototype");
				count++;
			}
		}
	}

	return count;
}
