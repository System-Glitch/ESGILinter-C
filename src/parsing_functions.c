#include <stdlib.h>
#include "arraylist.h"
#include "parsing_functions.h"
#include "scopetree.h"

function_t *function_init(char *name, unsigned char is_prototype, char *type, unsigned char type_is_pointer, arraylist_t *params) {
	function_t *function = malloc(sizeof(function_t));

	if(function != NULL) {
		function->name                   = name;
		function->is_prototype           = is_prototype;
		function->return_type.name       = type;
		function->return_type.is_pointer = type_is_pointer;
		function->params                 = params;
	}

	return function;
}

static unsigned char function_is_prototype(char *line) {
	size_t length = strlen(line);
	int index = length - 1;

	while(is_whitespace(line[index]) && index >= 0) { index--; }; //Skip whitespaces

	switch(line[index]) {
		case ';': return 1;
		case '{': return 0;
		default: return -1; //Syntax error
	}
}

static char *parse_function_name(char *line, unsigned int *star_count_name, unsigned int *params_start_index) {
	unsigned int index      = 0;
	unsigned int length     = strlen(line);
	unsigned int star_count = 0;
	unsigned int sub_index  = 0;
	match_t *match          = NULL;
	unsigned char c;
	char *name = NULL;
	char *tmp  = NULL;

	SKIP_WHITESPACES

	match = match_init();
	if(match == NULL) return NULL;
	else if(line[index] == ';' || line[index] == '{') {
		free(match);
		return NULL;
	}

	match->index_start = index;

	//Until not alphanumeric
	while((is_alphanumeric(c = line[index]) || c == '*') && index < length) {
		index++;
	}

	match->index_end = index;

	SKIP_WHITESPACES

	if(line[index] != '(') { //No parenthesis found, this is not a function delaration
		free(match);
		return NULL;
	}

	//No array modifier in name

	tmp = substr_match(line, *match);
	free(match);

	star_count = strcount(tmp, '*');
	sub_index  = strcountuntil(tmp, '*', 0, 1);

	//Remove stars
	name = strsubstr(tmp, sub_index, strlen(tmp) - star_count);
	free(tmp);

	*star_count_name = star_count;
	*params_start_index = index + 1;
	return name;
}

static arraylist_t *parse_function_parameters(char *line) {
	arraylist_t *list   = arraylist_init(5);
	unsigned int start_index  = 0;
	unsigned int index        = 0;
	unsigned int length       = strlen(line);
	unsigned char c;
	char *tmp;

	SKIP_WHITESPACES

	start_index = index;

	//Until comma
	while((c = line[index]) != ',' && index < length) {
		index++;
	}

	if(index < length) {

			tmp = strsubstr(line, start_index, index - start_index);

			free(tmp);

	} //Else end reached

	return list;
}

function_t *get_function_from_declaration(char *line) {
	unsigned int star_count_type;
	unsigned int star_count_name;
	unsigned int type_sub_index;
	unsigned int length;
	unsigned int type_length;
	unsigned int type_start_index   = 0;
	unsigned int params_start_index = 0;
	unsigned int index              = 0;
	function_t *function = NULL;
	arraylist_t *params  = NULL;
	match_t *match_type  = NULL;
	char *tmp_name;
	char *tmp_params;
	char *name = NULL;
	char *type;
	char is_prototype;

	line = str_remove_comments(line);

	is_prototype = function_is_prototype(line);
	if(is_prototype == -1) {
		free(line);
		free(match_type);
		return NULL;
	}

	match_type = parse_type(line);
	if(match_type == NULL) {
		free(line);
		return NULL;
	}

	type = substr_match(line, *match_type);
	type_start_index = match_type->index_start;
	free(match_type);

	if(type != NULL) {

		type_length = strlen(type);
		tmp_name    = strsubstr(line, type_start_index + type_length, strlen(line) - type_length);
		length      = strlen(tmp_name);

		if(length > 0) {
			star_count_type = strcount(type, '*');
			type_sub_index  = strcountuntil(type, '*', 1, 1);

			//Remove stars
			type[type_length - type_sub_index] = '\0';

			//No array modifier in type

			if(strcount(type, '*')) { //Contains stars inside the type name -> invalid type
				free(tmp_name);
				free(type);
				free(line);
				return NULL;
			}

			name = parse_function_name(tmp_name, &star_count_name, &params_start_index);

			if(name != NULL) {

				index = params_start_index;
				//Find closing parenthesis
				while(tmp_name[index] != ')' && index < length) {
					index++;
				}

				if(tmp_name[index] != ')') { //Syntax error, no closing parenthesis found
					free(tmp_name);
					free(name);
					free(type);
					free(line);
					return NULL;
				}

				tmp_params = strsubstr(tmp_name, params_start_index, index - params_start_index);

				params = parse_function_parameters(tmp_params);
				free(tmp_params);
				free(tmp_name);

				if(params != NULL) {
					function = function_init(name, is_prototype, strduplicate(type), star_count_type + star_count_name, params);
				} else {
					free(line);
					return NULL;
				}
			} else {
				free(tmp_name);
				free(type);
				free(line);
				return NULL;
			}

		}

		free(type);
	}

	free(line);

	return function;
}

void function_free(function_t *function) {
	free(function->name);
	for(unsigned int i = 0 ; i < function->params->size ; i++)
		field_free(function->params->array[i]);
	arraylist_free(function->params, 1);
	free(function);
}
