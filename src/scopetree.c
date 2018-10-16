#include <errno.h>
#include "scopetree.h"
#include "stringutils.h"

define_t *define_init(char *name, char *value) {
	define_t *define = malloc(sizeof(define_t));

	if(define != NULL) {
		define->name = name;
		define->value = value;
		//TODO get field type
		define->type.name = "";
		define->type.is_pointer = false;
	}

	return define;
}

function_t *function_init(char *name, char *type, unsigned char type_is_pointer, arraylist_t *params) {
	function_t *function = malloc(sizeof(function_t));

	if(function != NULL) {
		function->name                   = name;
		function->return_type.name       = type;
		function->return_type.is_pointer = type_is_pointer;
		function->params                 = params;
	}

	return function;
}

field_t *field_init(char *name, char *type, unsigned char type_is_pointer) {
	field_t *field = malloc(sizeof(field_t));

	if(field != NULL) {
		field->name                   = name;
		field->return_type.name       = type;
		field->return_type.is_pointer = type_is_pointer; 
	}

	return field;
}

scope_t *scope_init(scope_t *parent) {
	scope_t *scope = malloc(sizeof(scope_t));

	if(scope != NULL) {
		scope->child     = linkedlist_init();
		scope->variables = arraylist_init(10);
		scope->defines   = arraylist_init(10);
	}

	return scope;
}

field_t *get_field_from_string(char *field) {
	//TODO
	errno = ENOSYS;
	return NULL;
}

define_t *get_define_from_string(char *define) {
	//TODO
	errno = ENOSYS;
	return NULL;
}

arraylist_t *get_function_params(char *function_head) {
	//TODO
	errno = ENOSYS;
	return NULL;
}

scope_t *parse_scope(char *file, unsigned int start_line, scope_t *parent_scope) {
	//TODO
	errno = ENOSYS;
	return NULL;
}

char type_equals(type_t *type1, type_t *type2);

void define_free(define_t *define) {
	free(define->name);
	free(define->value);
	free(define);
}

void field_free(field_t *field) {
	free(field->name);
	free(field);
}

void function_free(function_t *function) {
	free(function->name);
	for(unsigned int i = 0 ; i < function->params->size ; i++)
		field_free(function->params->array[i]);
	arraylist_free(function->params);
	free(function);
}

static void scope_list_free(arraylist_t *list, void (*free_function)(void *value)) {
	for(unsigned int i = 0 ; i < list->size; i++) {
		free_function(list->array[i]);
		list->array[i] = NULL;
	}
	arraylist_free(list);
}

void scope_free(scope_t *scope) {
	node_t * current;

	//Don't free parent !
	
	//Recursively free children
	current = scope->child->head;
	while (current != NULL) {
		scope_free(current->value);
		current = current->next;
	}

	//Free functions
	for(unsigned int i = 0 ; i < scope->functions->size; i++) {
		function_free(scope->functions->array[i]);
		scope->functions->array[i] = NULL;
	}
	arraylist_free(scope->functions);

	//Free variables
	for(unsigned int i = 0 ; i < scope->variables->size; i++) {
		field_free(scope->variables->array[i]);
		scope->variables->array[i] = NULL;
	}
	arraylist_free(scope->variables);

	//Free defines
	for(unsigned int i = 0 ; i < scope->defines->size; i++) {
		free_function(scope->defines->array[i]);
		scope->defines->array[i] = NULL;
	}
	arraylist_free(scope->defines);

	free(scope);
}