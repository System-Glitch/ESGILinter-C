#ifndef MESSAGES_H
#define MESSAGES_H

typedef struct messages {
	arraylist_t *undeclared_variables;
	arraylist_t *undeclared_functions;
	arraylist_t *invalid_params;
	arraylist_t *invalid_calls;
	arraylist_t *variables_list; //Variable is removed when used
	arraylist_t *functions_list; //Function is removed when used
} messages_t;

#endif
