#include <errno.h>
#include <regex.h>
#include "scopetree.h"
#include "stringutils.h"

define_t *define_init(char *name, char *value) {
	define_t *define = malloc(sizeof(define_t));

	if(define != NULL) {
		define->name = name;
		define->value = value;
		//TODO get field type
		define->type.name = "";
		define->type.is_pointer = 0;
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
		field->type.name       = type;
		field->type.is_pointer = type_is_pointer; 
	}

	return field;
}

scope_t *scope_init(scope_t *parent) {
	scope_t *scope = malloc(sizeof(scope_t));

	if(scope != NULL) {
		scope->child     = linkedlist_init();
		scope->variables = arraylist_init(10);
		scope->defines   = arraylist_init(10);
		scope->from_line = -1;
		scope->to_line   = -1;
	}

	return scope;
}

field_t *get_field_from_string(char *field) {
	//TODO
	errno = ENOSYS;
	return NULL;
}

static char *get_define_type(char *define) {
	//Substring the value (after last space after name, until new line)
	
	//If empty, type void
	//If not supported (macro, multiline), return NULL

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

scope_t *parse_scope(arraylist_t *file, unsigned int start_line, scope_t *parent_scope) {
	//TODO
	errno = ENOSYS;
	return NULL;
}

field_t *get_variable_from_declaration(char *line) {
	regex_t regex;
	regmatch_t pmatch[9];
	int star_count_name, star_count_type;
	field_t *variable = NULL;
	char *name;
	char *tmp_name;
	char *type;
	char *array_def;

	if(exec_regex(&regex, REGEX_VARIABLE_DECLARATION, line, 9, &pmatch)) {

		type     = substr_regex_match(line, pmatch[2]);
		tmp_name = substr_regex_match(line, pmatch[4]);

		star_count_type = strcountuntil(type, '*', 1);
		star_count_name = strcountuntil(tmp_name, '*', 0);

		//Remove stars
		type[strlen(type) - star_count_type] = '\0';
		name = strsubstr(tmp_name, star_count_name, strlen(tmp_name) - star_count_name);
		free(tmp_name);

		if(pmatch[5].rm_eo != -1) { //Is array
			array_def = substr_regex_match(line, pmatch[5]);
			star_count_type += strcount(array_def, '[');
			free(array_def);
		}

		if(type != NULL && name != NULL)		
			variable = field_init(name, type, star_count_name + star_count_type);
	}

	regfree(&regex);

	return variable;
}

char type_equals(type_t *type1, type_t *type2) {
	//TODO handle synonyms such as unsigned char and uint8_t
	return !strcmp(type1->name, type2->name) && type1->is_pointer == type2->is_pointer;
}

void define_free(define_t *define) {
	free(define->name);
	free(define->value);
	free(define);
}

void field_free(field_t *field) {
	free(field->name);
	free(field->type.name);
	free(field);
}

void function_free(function_t *function) {
	free(function->name);
	for(unsigned int i = 0 ; i < function->params->size ; i++)
		field_free(function->params->array[i]);
	arraylist_free(function->params);
	free(function);
}

void scope_free(scope_t *scope) {
	node_t * current;

	//Don't free parent !
	
	//Recursively free children
	current = scope->child->head;
	while (current != NULL) {
		scope_free(current->val);
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
		define_free(scope->defines->array[i]);
		scope->defines->array[i] = NULL;
	}
	arraylist_free(scope->defines);

	free(scope);
}
