#include <errno.h>
#include "scopetree.h"
#include "stringutils.h"
#include "parsing_variables.h"
#include "parsing_functions.h"

field_t *field_init(char *name, char *type, char type_is_pointer, int line_index) {
	field_t *field = malloc(sizeof(field_t));

	if(field != NULL) {
		field->name            = name;
		field->type.name       = type;
		field->type.is_pointer = type_is_pointer;
		field->type.is_literal = 0;
		field->line            = line_index;
		field->is_param        = 0;
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
		scope->from_line = -1;
		scope->to_line   = -1;
		scope->from_char = -1;
		scope->to_char   = -1;
	}

	return scope;
}

scope_t *is_in_child_scope(scope_t *scope, int line) {
	scope_t *child;
	node_t * current = scope->children->head;
	if(current != NULL) {
		do {
			child = (scope_t*) current->val;
			if(line >= child->from_line && line <= child->to_line)
				return child;
		} while ((current = current->next) != NULL);
	}

	return NULL;
}

static char previous_char_is_equal(char *line, int index) {
	char c;

	index--;
	while(is_whitespace(c = line[index]) && index >= 0) { index--; }

	return c == '=';
}

static void parse_scope_content(arraylist_t *file, scope_t *scope) {
	char *line;
	arraylist_t *variables;

	for(int i = scope->from_line ; i <= scope->to_line ; i++) {
		if(is_in_child_scope(scope, i)) continue; //Ignore lines inside child scopes
		line = (char*) arraylist_get(file, i);
		if(i != scope->from_line)
			variables = get_variables_from_declaration(i,line);
		else
			variables = get_variables_from_declaration(i,line + scope->from_char);
		if(variables != NULL) {
			arraylist_add_all(scope->variables, variables);
			arraylist_free(variables, 0);
			variables = NULL;
		}
	}
}

static void parse_scope_functions(arraylist_t *file, scope_t *scope) {
	char *line;
	function_t *function;

	for(int i = scope->from_line ; i < scope->to_line ; i++) {
		line = (char*) arraylist_get(file, i);
		function = get_function_from_declaration(i, line);
		if(function != NULL) {
			arraylist_add(scope->functions, function);
			function = NULL;
		}
	}
}

static function_t *find_function_from_line(arraylist_t *list, int line) {
	function_t *function = NULL;
	for(size_t i = 0 ; i < list->size ; i++) {
		function = arraylist_get(list, i);
		if(function->line == line)
			return function;
	}
	return NULL;
}

scope_t *parse_root_scope(arraylist_t *file) {
	scope_t    *scope     = scope_init(NULL);
	scope_t    *child     = NULL;
	scope_t    *tmp_child = NULL;
	function_t *function  = NULL;
	node_t     *current   = NULL;
	field_t    *tmp_field = NULL;
	field_t    *field     = NULL;

	if(scope == NULL) return NULL;

	scope->from_line = 0;
	scope->to_line   = file->size - 1;
	scope->from_char = 0;
	scope->to_char   = strlen((char*) arraylist_get(file, file->size - 1));

	while((child = parse_scope(file, tmp_child != NULL ? tmp_child->to_line : 0, tmp_child != NULL ? tmp_child->to_char : 0, 0, scope)) != NULL) {
		linkedlist_add(scope->children, child);
		tmp_child = child;
	}

	parse_scope_content(file, scope);
	parse_scope_functions(file, scope); //Parse functions only in root scope

	//Copy parameters to child variables
	current = scope->children->head;
	if(current != NULL) {
		do {
			tmp_child = current->val;
			function = find_function_from_line(scope->functions, tmp_child->from_line);
			if(function != NULL) {
				for(size_t i = 0 ; i < function->params->size ; i++) {
					tmp_field = arraylist_get(function->params, i);
					field = field_init(strduplicate(tmp_field->name), strduplicate(tmp_field->type.name), tmp_field->type.is_pointer, tmp_field->line);
					field->is_param = 1;
					arraylist_add(tmp_child->variables, field);
				}
			}
		} while ((current = current->next) != NULL);
	}

	return scope;
}

static int check_switch_control(char *line, int index, int length) {

	char c;

	SKIP_WHITESPACES

	if(strstr(line + index, "case") == line + index || strstr(line + index, "default") == line + index) {
		index += c == 'c' ? 4 : 7;

		SKIP_WHITESPACES

		while((c = line[index]) != ':' && index < length) {
			index++;
		}

		if(c != ':') return -1;

		return index + 1;
	}

	return -1;

}

static unsigned char starts_with_switch(char *line, int length) {
	int index = 0;
	char c;

	SKIP_WHITESPACES

	if(strstr(line + index, "switch") == line + index) {
		index += 6;

		SKIP_WHITESPACES

		if(c == '(')
			return 1;
	}

	return 0;
}

scope_t *parse_scope(arraylist_t *file, unsigned int start_line, unsigned int from_char, unsigned char in_case, scope_t *parent_scope) {
	scope_t *scope              = scope_init(parent_scope);
	scope_t *child              = NULL;
	unsigned int level          = in_case ? 1 : 0;
	unsigned int no_scope_level = 0;
	int case_start              = 0;
	char found                  = 0;
	unsigned char in_switch     = 0;
	unsigned int length;
	char *line;

	if(in_case) {
		scope->from_line = start_line;
		scope->from_char = from_char;
	}

	for(size_t i = start_line ; i < file->size ; i++) { //Already considers end of line as bracket
		line   = str_remove_comments(arraylist_get(file, i));
		length = strlen(line);
		for(size_t j = i == start_line ? from_char : 0 ; j < length ; j++) {

			if(!check_quotes(line, line + j, length)) {

				if(in_switch || in_case) {
					case_start = check_switch_control(line, j, length);
					if(case_start != -1) {
						if(in_case) {
							level--;

							if(level == 0) {
								scope->to_line = in_case == i ? i : i-1;
								scope->to_char = j;
								if(in_case != i)
									break;
							}
							in_case = 0;
							j--;
						} else {
							level++;
							in_case = i;
							j = case_start;
							if(level == 2) {
								child = parse_scope(file, i, j, in_case, scope);
								if(child != NULL)
									linkedlist_add(scope->children, child);
							}
						}
					}
				}

				if(line[j] == '{') {

					if(previous_char_is_equal(line, j)) {
						no_scope_level++;
					} else {
						level++;

						if(!found) {
							scope->from_line = i;
							scope->from_char = j;
							in_switch = starts_with_switch(line, length) ? level : 0;
						}

						found = 1;

						if(!in_switch && level == 2) {
							child = parse_scope(file, i, j, 0, scope);
							if(child != NULL)
								linkedlist_add(scope->children, child);
						}
					}
				} else if(line[j] == '}') {
					if(no_scope_level != 0) {
						no_scope_level--;
					} else {
						level--;
						if(in_case && level == 0) {

							in_case = 0;
							if(level == 0) {
								scope->to_line = i-1;
								scope->to_char = 0;
								break;
							}
							in_switch = 1;
						}

						if((found || in_switch) && level == 0) {
							scope->to_line = i;
							scope->to_char = j + 1;
							break;
						}
					}
				}
			}
		}

		free(line);
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

unsigned char type_equals(type_t *type1, type_t *type2) {
	//TODO handle synonyms such as unsigned char and uint8_t
	return !strcmp(type1->name, type2->name) && type1->is_pointer == type2->is_pointer;
}

unsigned char type_exists(char *type) {
	static const char *known_types[] = {"char","short","int","unsigned","signed","long","signed","float","double","size_t","void"};
	for(unsigned i = 0 ; i < 11 ; i++)
		if(!strcmp(type, known_types[i]))
			return 1;

	return 0;
}

void field_free(field_t *field) {
	free(field->name);
	free(field->type.name);
	free(field);
}

void field_list_free(arraylist_t *list) {
	field_t *field = NULL;
	for(unsigned int i = 0 ; i < list->size ; i++) {
		field = arraylist_get(list, i);
		field_free(field);
	}
	arraylist_free(list, 0);
}

void function_list_free(arraylist_t *list) {
	function_t *function = NULL;
	for(unsigned int i = 0 ; i < list->size ; i++) {
		function = arraylist_get(list, i);
		function_free(function);
	}
	arraylist_free(list, 0);
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
	field_list_free(scope->variables);

	free(scope);
}

scope_t *get_root_scope(scope_t *scope) {
	scope_t *parent = NULL;
	while((parent = scope->parent) != NULL) {
		scope = parent;
	}
	return scope;
}

function_t *find_function(scope_t *scope, char *name, unsigned char allow_prototypes) {
	scope_t *root = get_root_scope(scope);
	function_t *function = NULL;
	for(size_t i = 0 ; i < root->functions->size ; i++) {
		function = arraylist_get(root->functions, i);
		if(!strcmp(name, function->name) && (allow_prototypes || !function->is_prototype))
			return function;
	}
	return NULL;
}

field_t *find_variable(scope_t *scope, char *name) {
	field_t *variable = NULL;

	while(scope != NULL) {
		for(size_t i = 0 ; i < scope->variables->size ; i++) {
			variable = arraylist_get(scope->variables, i);
			if(!strcmp(name, variable->name))
				return variable;
		}
		scope = scope->parent;
	}
	return NULL;
}

function_t *find_function_prototype(scope_t *root_scope, char *name) {

	arraylist_t *functions = root_scope->functions;
	function_t *prototype  = NULL;

	for(size_t j = 0 ; j < functions->size ; j++) {
		prototype = arraylist_get(functions, j);
		if(prototype->is_prototype && !strcmp(prototype->name, name)) {
			return prototype;
		}
	}

	return NULL;
}

unsigned char is_keyword(char *name) {
	static const char *keywords[] = {
		"auto", "break", "case", "char", "const", "continue",
		"default", "do", "double", "else", "enum", "extern",
		"float", "for", "goto", "if", "inline", "int", "long",
		"register", "restrict", "return", "short", "signed",
		"sizeof", "static", "struct", "switch", "typedef", "union",
		"unsigned", "void", "volatile", "while",
		NULL
	};
	const char *word   = NULL;
	size_t index = 0;
	while((word = keywords[index++]) != NULL) {
		if(!strcmp(name, word))
			return 1;
	}
	return 0;
}
