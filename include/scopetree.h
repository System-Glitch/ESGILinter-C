#ifndef SCOPETREE_H
#define SCOPETREE_H

#include "linkedlist.h"
#include "arraylist.h"
#include "parsing_type.h"

#define SKIP_WHITESPACES while(is_whitespace(c = line[index]) && index < length) { index++; };

typedef struct field {
	char   *name;
	type_t  type;
} field_t;

typedef struct function {
	char         *name;
	type_t        return_type;
	unsigned char is_prototype;
	arraylist_t  *params;
} function_t;

typedef struct scope {
	struct scope *parent;
	linkedlist_t *children;
	arraylist_t  *functions;
	arraylist_t  *variables;
	arraylist_t  *defines;
	int from_line;
	int from_char;
	int to_line;
	int to_char;
} scope_t;

field_t     *field_init(char *name, char *type, unsigned char type_is_pointer);
scope_t     *scope_init(scope_t *parent);

field_t     *get_field_from_string(char *field);
arraylist_t *get_function_params(char *function_head);

scope_t     *parse_root_scope(arraylist_t *file);
scope_t     *parse_scope(arraylist_t *file, unsigned int start_line, unsigned int from_char, scope_t *parent_scope);

unsigned char type_equals(type_t *type1, type_t *type2);
unsigned char type_exists(char *type);

void field_free(field_t *field);
void field_list_free(arraylist_t *list);
void scope_free(scope_t *scope);

#endif
