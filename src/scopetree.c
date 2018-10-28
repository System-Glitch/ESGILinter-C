#include <errno.h>
#include "scopetree.h"
#include "stringutils.h"
#include "parsing_variables.h"
#include "parsing_defines.h"

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
		scope->parent    = parent;
		scope->children  = linkedlist_init();
		scope->variables = arraylist_init(10);
		scope->functions = arraylist_init(10);
		scope->defines   = arraylist_init(10);
		scope->from_line = -1;
		scope->to_line   = -1;
		scope->from_char = -1;
		scope->to_char   = -1;
	}

	return scope;
}

field_t *get_field_from_string(char *field) {
	//TODO
	errno = ENOSYS;
	return NULL;
}

arraylist_t *get_function_params(char *function_head) {
	//TODO
	errno = ENOSYS;
	return NULL;
}

static char is_in_child_scope(scope_t *scope, int line) {
	scope_t *child;
	node_t * current = scope->children->head;
	if(current != NULL) {
		do {
			child = (scope_t*) current->val;
			if(line >= child->from_line && line <= child->to_line)
				return 1;
		} while ((current = current->next) != NULL);
	}

	return 0;
}

static char previous_char_is_equal(char *line, int index) {
	char c;

	index--;
	while(is_whitespace(c = line[index]) && index >= 0) { index--; }

	return c == '=';
}

static void parse_scope_content(arraylist_t *file, scope_t *scope) {
	char *line;
	arraylist_t *list;

	for(int i = scope->from_line ; i < scope->to_line ; i++) {
		if(is_in_child_scope(scope, i)) continue; //Ignore lines inside child scopes
		line = (char*) arraylist_get(file, i);
		list = get_variables_from_declaration(line);
		if(list != NULL) {
			arraylist_add_all(scope->variables, list);
			arraylist_free(list, 0);
			list = NULL;
		}
	}
}

scope_t *parse_root_scope(arraylist_t *file) {
	scope_t *scope        = scope_init(NULL);
	scope_t *child        = NULL;
	scope_t *tmp_child    = NULL;

	if(scope == NULL) return NULL;

	scope->from_line = 0;
	scope->to_line   = file->size - 1;
	scope->from_char = 0;
	scope->to_char   = strlen((char*) arraylist_get(file, file->size - 1));

	while((child = parse_scope(file, tmp_child != NULL ? tmp_child->to_line : 0, tmp_child != NULL ? tmp_child->to_char : 0, scope)) != NULL) {
		linkedlist_add(scope->children, child);
		tmp_child = child;
	}

	parse_scope_content(file, scope);

	return scope;
}

scope_t *parse_scope(arraylist_t *file, unsigned int start_line, unsigned int from_char, scope_t *parent_scope) {
	scope_t *scope              = scope_init(parent_scope);
	unsigned int level          =  0;
	unsigned int no_scope_level =  0;
	char found                  =  0;
	char in_comment             =  0;
	unsigned int length;
	char *line;

	for(size_t i = start_line ; i < file->size ; i++) { //Already considers end of line as bracket
		line   = arraylist_get(file, i);
		length = strlen(line);
		for(size_t j = i == start_line ? from_char : 0 ; j < length ; j++) {
			
			if(line[j] == '/' && line[j+1] == '/') {
				//Comment until end of line, skip line
				break;
			}

			if(in_comment) { //Close multi-line comment
				if(line[j] == '*' && line[j+1] == '/') {
					in_comment = 0;
					j += 2;
				}
			} else {
				if(line[j] == '/' && line[j+1] == '*') { //Open multi-line comment
					in_comment = 1;
				} else {
					if(line[j] == '{') {

						if(previous_char_is_equal(line, j)) {
							no_scope_level++;
						} else {
							level++;

							if(!found) {
								scope->from_line = i;
								scope->from_char = j;
							}

							found = 1;

							if(level > 1) {
								scope_t *child = parse_scope(file, i, j, scope);
								if(child != NULL)
									linkedlist_add(scope->children, child);
							}
						}
					} else if(line[j] == '}') {
						if(no_scope_level != 0) {
							no_scope_level--;
						} else {
							level--;

							if(found && level == 0) {
								scope->to_line = i;
								scope->to_char = j + 1;
								break;
							}
						}
					}
				}
			}
		}

		if(scope->to_line != -1) break;
	}

	if(scope->to_line == -1) {
		scope_free(scope);
		scope = NULL;
	} else {
		parse_scope_content(file, scope);
	}

	return scope;
}

char type_equals(type_t *type1, type_t *type2) {
	//TODO handle synonyms such as unsigned char and uint8_t
	return !strcmp(type1->name, type2->name) && type1->is_pointer == type2->is_pointer;
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
	arraylist_free(function->params, 1);
	free(function);
}

void scope_free(scope_t *scope) {
	node_t * current;

	//Don't free parent !
	
	//Recursively free children
	current = scope->children->head;
	while (current != NULL) {
		scope_free(current->val);
		current->val = NULL;
		current = current->next;
	}
	linkedlist_free(scope->children);

	//Free functions
	for(size_t i = 0 ; i < scope->functions->size; i++) {
		function_free(scope->functions->array[i]);
		scope->functions->array[i] = NULL;
	}
	arraylist_free(scope->functions, 1);

	//Free variables
	for(size_t i = 0 ; i < scope->variables->size; i++) {
		field_free(scope->variables->array[i]);
		scope->variables->array[i] = NULL;
	}
	arraylist_free(scope->variables, 1);

	//Free defines
	for(size_t i = 0 ; i < scope->defines->size; i++) {
		define_free(scope->defines->array[i]);
		scope->defines->array[i] = NULL;
	}
	arraylist_free(scope->defines, 1);

	free(scope);
}
