#ifndef PARSING_FUNCTIONS_H
#define PARSING_FUNCTIONS_H

#include "stringutils.h"
#include "scopetree.h"

typedef struct invalid_call { //Used to know if a function call is missing params or have too many params
	char *name;
	int   more; //If negative -> missing parameters, if positive -> too many
} invalid_call_t;

function_t *function_init(char *name, unsigned char is_prototype, char *type, unsigned char type_is_pointer, arraylist_t *params, int line_index);
function_t *get_function_from_declaration(int line_index, char *line);
function_t *parse_function_call(int line_index, char *line);
void        check_function_call_parameters(scope_t *scope, function_t *call, function_t *function, int line_index, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid, arraylist_t *variables_list, arraylist_t *functions_list, arraylist_t *invalid_calls);

void function_free(function_t *function);

#endif
