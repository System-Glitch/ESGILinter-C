#ifndef MESSAGES_H
#define MESSAGES_H

#include "arraylist.h"
#include "scopetree.h"

typedef struct messages {
	arraylist_t *undeclared_variables;
	arraylist_t *undeclared_functions;
	arraylist_t *invalid_params;
	arraylist_t *invalid_calls;
	arraylist_t *wrong_assignment;
	arraylist_t *wrong_return;
	arraylist_t *ternary_types;
	arraylist_t *variables_list; //Variable is removed when used
	arraylist_t *functions_list; //Function is removed when used
} messages_t;

typedef struct wrong_type {
	type_t *expected_type;
	type_t *actual_type;
} wrong_type_t;

#endif
