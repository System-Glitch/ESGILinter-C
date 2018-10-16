#ifndef SCOPETREE_H
#define SCOPETREE_H

#include "linkedlist.h"
#include "arraylist.h"

typedef struct type {
	char          *name;
	unsigned char  is_pointer;
} type_t;

typedef struct field {
	char   *name;
	type_t  return_type;
} field_t;

typedef struct function {
	char        *name;
	type_t       return_type;
	arraylist_t *params;
} function_t;

typedef struct define {
	char   *name;
	char   *value;
	type_t  type; //Needs to be identified
} define_t;

typedef struct scope {
	struct scope *parent;
	linkedlist_t *child;
	arraylist_t  *functions;
	arraylist_t  *variables;
	arraylist_t  *defines;
} scope_t;


define_t   *define_init(char *name, char *value);
function_t *function_init(char *name, char *type, unsigned char type_is_pointer, arraylist_t *params);
field_t    *field_init(char *name, char *type, unsigned char type_is_pointer);
scope_t    *scope_init(scope_t *parent);

field_t     *get_field_from_string(char *field);
define_t    *get_define_from_string(char *define);
arraylist_t *get_function_params(char *function_head);

scope_t *parse_scope(char *file, unsigned int start_line, scope_t *parent_scope);

char type_equals(type_t *type1, type_t *type2);

void define_free(define_t *define);
void field_free(field_t *field);
void function_free(function_t *function);
void scope_free(scope_t *scope);

#endif