#include "rules/no_prototype.h"
#include "arraylist.h"
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
	char found             = 0;
	unsigned int count     = 0; //Counts warnings and errors

	for(size_t i = 0 ; i < functions->size ; i++) {
		found = 0;
		function = arraylist_get(functions, i);
		if(!function->is_prototype && strcmp(function->name, "main")) {
			for(size_t j = 0 ; j < functions->size ; j++) {
				if(i == j) continue;
				prototype = arraylist_get(functions, j);
				if(prototype->is_prototype && !strcmp(prototype->name, function->name)) {
					//Proto found

					//Check parameters and return type
					if(strcmp(function->return_type.name, prototype->return_type.name) || !check_params(function, prototype)) {
						print_error("fictive_file.c", prototype->line, arraylist_get(file, prototype->line), "Conflicting types");
						count++;
					}
					found = 1;
					break;
				}
			}

			if(!found) {
				//Print warning
				print_warning("fictive_file.c", function->line, arraylist_get(file, function->line), "Missing prototype");
				count++;
			}
		}
	}

	return count;
}
