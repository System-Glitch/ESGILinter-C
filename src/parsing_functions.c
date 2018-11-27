#include <stdlib.h>
#include "arraylist.h"
#include "scopetree.h"
#include "parsing_type.h"
#include "parsing_functions.h"
#include "parsing_variables.h"
#include "parsing_expressions.h"

function_t *function_init(char *name, unsigned char is_prototype, char *type, unsigned char type_is_pointer, arraylist_t *params, int line_index) {
	function_t *function = malloc(sizeof(function_t));

	if(function != NULL) {
		function->name                   = name;
		function->is_prototype           = is_prototype;
		function->return_type.name       = type;
		function->return_type.is_pointer = type_is_pointer;
		function->params                 = params;
		function->line                   = line_index;
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

static arraylist_t *parse_function_parameters(char *line, unsigned char is_prototype, int line_index) {
	arraylist_t *list            = arraylist_init(5);
	unsigned int start_index     = 0;
	unsigned int index           = 0;
	unsigned int sub_index       = 0;
	unsigned int length          = strlen(line);
	unsigned int type_length     = 0;
	unsigned int star_count_type = 0;
	unsigned int star_count_name = 0;
	unsigned int array_count     = 0;
	unsigned int type_sub_index;
	unsigned char c;
	char *tmp           = NULL;
	char *tmp_name      = NULL;
	char *type          = NULL;
	char *name          = NULL;
	match_t *match_type = NULL;
	match_t *match_name = NULL;
	field_t *field      = NULL;

	do {
		SKIP_WHITESPACES

		start_index = index;

		//Until comma
		while((c = line[index]) != ',' && index < length) {
			index++;
		}

		tmp = strsubstr(line, start_index, index - start_index);
		if(strlen(tmp) == 0) {
			free(tmp);
			break;
		}

		//Parse type
		match_type = parse_type(tmp);
		if(match_type == NULL) {
			free(tmp);
			field_list_free(list);
			return NULL;
		}

		type = substr_match(tmp, *match_type);
		free(match_type);
		type_length     = strlen(type);
		star_count_type = strcount(type, '*');
		type_sub_index  = strcountuntil(type, '*', 1, 1);

		//Remove stars
		type[type_length - type_sub_index] = '\0';

		//If no name and not prototype, invalid syntax
		match_name = parse_variable_name(tmp + type_length, &sub_index, &array_count);
		if(match_name != NULL) {
			tmp_name = substr_match(tmp + type_length, *match_name);
			free(match_name);

			if(strlen(tmp_name) == 0 && !is_prototype) { //Parameter names compulsory when not prototype
				free(tmp);
				field_list_free(list);
				return NULL;
			}

			star_count_name = strcount(tmp_name, '*');
			sub_index       = strcountuntil(tmp_name, '*', 0, 1);

			//Remove stars
			name = strsubstr(tmp_name, sub_index, strlen(tmp_name) - star_count_name);
			free(tmp_name);
		} else if(!is_prototype) { //Parameter names compulsory when not prototype
			free(tmp);
			field_list_free(list);
			return NULL;
		}

		field = field_init(name, type, star_count_type + star_count_name + array_count, line_index);
		field->is_param = 1;
		arraylist_add(list, field);

		free(tmp);

		index++;
		
	} while(index < length);

	return list;
}

function_t *get_function_from_declaration(int line_index, char *line) {
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
	char *type = NULL;
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

				params = parse_function_parameters(tmp_params, is_prototype, line_index);
				free(tmp_params);
				free(tmp_name);

				if(params != NULL) {
					function = function_init(name, is_prototype, strduplicate(type), star_count_type + star_count_name, params, line_index);
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

static arraylist_t *parse_function_call_params(char *line) {
	arraylist_t *list        = arraylist_init(5);
	unsigned int start_index = 0;
	unsigned int end_index   = 0;
	unsigned int index       = 0;
	size_t length            = strlen(line);
	char *name               = NULL;
	char c;

	if(list == NULL || length == 0) return list;

	do {
		SKIP_WHITESPACES

		start_index = index;

		//Until comma or end
		while((c = line[index]) != ',' && index < length) {
			index++;
		}

		end_index = index;

		name = strsubstr(line, start_index, end_index - start_index);
		if(type_exists(name)) {
			//Syntax error
			arraylist_free(list, 1);
			return NULL;
		}
		arraylist_add(list, name);

		SKIP_WHITESPACES

		if(index < length && c != ',') {
			//Expected comma but get something else
			//Syntax error
			arraylist_free(list, 1);
			return NULL;
		} else if(c == ',') {
			//Expect next param
			index++;
		}
	} while(index < length);

	if(is_whitespace(c) || c == ',') {
		//Expected next param but reached end
		//Syntax error
		arraylist_free(list, 1);
		list = NULL;
	}

	return list;
}

function_t *parse_function_call(int line_index, char *line) {
	//Line should be already subbed if call is assignation
	function_t  *function     = NULL;
	arraylist_t *param_list   = NULL;
	int start_index           = 0;
	int end_index             = 0;
	unsigned int index        = 0;
	char *name                = NULL;
	char *params              = NULL;
	char *expr                = NULL;
	size_t length             = 0;
	char c;

	line   = str_remove_comments(line);
	length = strlen(line);

	SKIP_WHITESPACES

	start_index = index;

	//Until not alphanumeric
	while(is_alphanumeric(c = line[index]) && index < length) {
		index++;
	}

	end_index = index;

	name = strsubstr(line, start_index, end_index - start_index);

	SKIP_WHITESPACES

	//Expect parenthesis
	if(c == '(') {
		//Find closing parenthesis
		start_index = index + 1;
		end_index = strlastindexof(line, ')');
		if(end_index != -1 && end_index >= start_index) {

			index = end_index + 1;

			//Check if parenthesis is followed by anything
			SKIP_WHITESPACES
			if(index < length && c != ';' && !is_whitespace(c)) {
				free(name);
				free(line);
				return NULL;
			}


			params = strsubstr(line, start_index, end_index - start_index);
			param_list = parse_function_call_params(params);
			free(params);
			if(param_list != NULL) {
				function = function_init(name, 0, strduplicate("void"), 0,  arraylist_init(param_list->size), line_index);
				function->params->size = param_list->size;
				for(size_t i = 0 ; i < param_list->size ; i++) {
					expr = arraylist_get(param_list, i);
					function->params->array[i] = field_init(expr, strduplicate("void"), 0, -1);
					((field_t*)function->params->array[i])->type = parse_expression(expr, line_index, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				}
				free(line);
				return function;
			}
		}
	}

	free(name);
	free(line);

	return NULL;
}

void check_function_call_parameters(scope_t *scope, function_t *call, function_t *function, int line_index, char *line, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid, arraylist_t *variables_list, arraylist_t *functions_list, arraylist_t *invalid_calls) {
	field_t *field            = NULL;
	field_t *param            = NULL;
	field_t *var_dec          = NULL;
	function_t* function_call = NULL;
	function_t* function_dec  = NULL;
	invalid_call_t *error     = NULL;

	if(function != NULL && call->params->size != function->params->size && invalid_calls != NULL) {
		error = malloc(sizeof(invalid_call_t));
		error->name = strduplicate(function->name);
		error->more = call->params->size - function->params->size;
		arraylist_add(invalid_calls, error);
	}

	for(size_t i = 0 ; i < call->params->size ; i++) {

		field   = arraylist_get(call->params, i);			
		var_dec = find_variable(scope, field->name);

		if(var_dec == NULL) {
			function_call = parse_function_call(line_index, field->name); //Maybe it's a function
			if(function_call != NULL) {
				function_dec = find_function(scope, function_call->name, 0);
				if(function_dec != NULL && (function_dec->line < line_index || find_function_prototype(get_root_scope(scope), function_call->name) != NULL)) {
					if(functions_list) {
						arraylist_remove(functions_list, arraylist_index_of(functions_list, function_dec));
					}
					check_function_call_parameters(scope, function_call, function_dec, line_index, line, undeclared_variables, undeclared_functions, invalid, variables_list, functions_list, invalid_calls);
					function_free(function_call);
				} else {
					arraylist_add(undeclared_functions, function_call);
					check_function_call_parameters(scope, function_call, function_dec, line_index, line, undeclared_variables, undeclared_functions, invalid, variables_list, functions_list, invalid_calls);
				}
			} else if(!field->type.is_literal) {
				arraylist_add(undeclared_variables, strduplicate(field->name));
			} else if(function != NULL && field->type.is_literal && i < function->params->size) {
				param = arraylist_get(function->params, i);
				if(!type_equals(&(field->type), &(param->type))) {
					arraylist_add(invalid, field_init(strduplicate(field->name), strduplicate(field->type.name), field->type.is_pointer, field->line));
				}
			}
		} else if(function != NULL) {
			if(functions_list) {
				arraylist_remove(variables_list, arraylist_index_of(variables_list, var_dec));
			}
			if(i < function->params->size) {
				param = arraylist_get(function->params, i);
				if(!type_equals(&(field->type), &(param->type))) {
					arraylist_add(invalid, field_init(strduplicate(field->name), strduplicate(field->type.name), field->type.is_pointer, field->line));
				}
			}
		}
	}
}

void function_free(function_t *function) {
	free(function->name);
	field_list_free(function->params);
	free(function);
}
