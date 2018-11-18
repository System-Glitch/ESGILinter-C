#include <stdlib.h>
#include <string.h>
#include "parsing_type.h"
#include "parsing_functions.h"

static char parse_type_word(char *line, size_t length, unsigned int *i) {
	char c;
	unsigned int index  = *i;
	int type_word_start =  0;
	int type_word_end   =  0;
	char found          =  0;
	char *word;

	do {

		SKIP_WHITESPACES

		type_word_start = index;
		while((is_alphanumeric(c = line[index]) || c == '*') && index < length) {
			index++;
		}
		type_word_end = index;

		word = strsubstr(line, type_word_start, type_word_end - type_word_start);

		//Remove stars
		word[strlen(word) - strcountuntil(word, '*', 1, 1)] = '\0';
		if(!type_exists(word)) {
			free(word);
			index = type_word_start;
			break;
		} else found = 1;
		free(word);
	} while(c != ';');

	*i = index;
	return found;
}

match_t *parse_type(char *line) {
	unsigned int index = 0;
	int type_start = -1;
	int type_end = -1;

	size_t length = strlen(line);
	unsigned char c;
	match_t *match = NULL;

	if(length < 3) return NULL; //3 is the minimum size for a declaration

	SKIP_WHITESPACES

	if(strstr(line + index, "extern") == line + index) //Skip extern keyword
		index += 6;

	SKIP_WHITESPACES

	if(strstr(line + index, "static") == line + index) //Skip static keyword
		index += 6;

	SKIP_WHITESPACES

	if(strstr(line + index, "const") == line + index) //Skip const keyword
		index += 5;

	SKIP_WHITESPACES

	type_start = index;

	if(!parse_type_word(line, length, &index)) //No type word found, it's not a variable declaration
		return NULL;

	if(line[index] == ';') //If found semi-colon, it's not a variable declaration
		return NULL;

	//Find star	
	while(index < length) {
		SKIP_WHITESPACES

		//If found something else than star, end
		if(c != '*') {
			index--;
			break;
		} else if(c == ';') //If found semi-colon, it's not a variable declaration
			return NULL;
		else
			index++;
	}

	type_end = index;

	if(type_end != -1) {
		match = match_init();
		if(match != NULL) {
			match->index_start = type_start;
			match->index_end = type_end;
		}
	}

	return match;
}

static void parse_variable_expression(char *line, char **name, char *is_pointer) {
	unsigned int start_index = 0;
	unsigned int end_index   = 0;
	int index                = 0;
	int length               = strlen(line);
	char c;

	//Pre incrementing / decrementing
	SKIP_WHITESPACES
	if(strstr(line + index, "++") == line + index || strstr(line + index, "--") == line + index) {
		//Skip operation
		index++;	
	} else if(index >= 0) {
		index--;
		c = line[index];
	}

	//Check referencing
	do {
		index++;
		SKIP_WHITESPACES

		if(c == '*') (*is_pointer)--;
		if(c == '&') (*is_pointer)++;
	} while(c == '*' || c == '&');

	SKIP_WHITESPACES

	start_index = index;

	//Until not alphanumeric
	while(is_alphanumeric(c = line[index]) && index < length) {
		index++;
	}

	end_index = index;

	*name = strsubstr(line, start_index, end_index - start_index);

}

type_t get_expression_type(char *line, int line_index, scope_t *scope, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params) { //TODO rename to parse_expression
	type_t type;
	function_t *function     = NULL;
	function_t *function_dec = NULL;
	field_t *variable_dec    = NULL;
	char *variable_name      = NULL;
	char is_pointer          = 0;
	unsigned int start_index = 0;
	unsigned int end_index   = 0;
	int index                = 0;
	unsigned int sub_index   = 0;
	int length               = strlen(line);
	int type_length;
	char c;

	type.name = "NULL";
	type.is_pointer = 0;

	SKIP_WHITESPACES

	if(c == '(') {
		//Parenthesis -> cast

		index++;

		SKIP_WHITESPACES

		start_index = index;

		while((c = line[index]) != ')' && index < length) index++;

		if(c != ')') { //No closing parenthesis, syntax error
			return type;
		}

		//Skip whitespaces backwards to find end of type
		while(is_whitespace(c = line[index]) && index > 0) { index--; }

		end_index = index;

		type_length     = end_index - start_index;
		type.name       = strsubstr(line, start_index, type_length);
		type.is_pointer = strcount(type.name, '*');
		sub_index       = strcountuntil(type.name, '*', 1, 1);

		//Remove stars
		type.name[type_length - sub_index] = '\0';

	} else if(c == '\'' && strlastindexof(line, '\'') != index) {
		type.name       = strduplicate("char");
		type.is_pointer = 0;
	} else if(c == '"' && strlastindexof(line, '"') != index) {
		type.name       = strduplicate("char");
		type.is_pointer = 1;
	} else {

		if(!is_digit(c)) {

			function = parse_function_call(line_index, line + index);
			if(function != NULL) {
				if(scope == NULL)
					arraylist_add(undeclared_functions, function);
				else {

					function_dec = find_function(scope, function->name, 0);
					if(function_dec != NULL && (function_dec->line < line_index || find_function_prototype(get_root_scope(scope), function->name) != NULL)) {
						type.name = strduplicate(function_dec->return_type.name);
						type.is_pointer = function_dec->return_type.is_pointer;
						check_function_call_parameters(scope, function, function_dec, line_index, line, undeclared_variables, undeclared_functions, invalid_params);
						function_free(function);
					} else {
						arraylist_add(undeclared_functions, function);
						check_function_call_parameters(scope, function, function_dec, line_index, line, undeclared_variables, undeclared_functions, invalid_params);
					}
				}

			} else {
				parse_variable_expression(line + index, &variable_name, &is_pointer);
				if(variable_name != NULL) {

					if(scope == NULL || (variable_dec = find_variable(scope, variable_name)) == NULL)
						arraylist_add(undeclared_variables, variable_name);
					else {
						type.is_pointer = variable_dec->type.is_pointer + is_pointer;
						if(type.is_pointer < 0) { //More dereferencing than allowed
							type.name = "NULL";
							type.is_pointer = 0;
						} else {
							type.name = strduplicate(variable_dec->type.name);
						}
					}
				}
			}
		} else {
			//TODO numbers
		}

	}


	//Starts with 0x -> integer
	//Integer -> only digits
	//ends with u or U -> unsigned
	//follows L or l -> long (while, max 2)

	//Double -> only digits, one dot, can end with 'd'
	//Can start with dot
	//follows L or l -> long (max 1)
	//float -> only digits, one dot, ends with 'f'

	//TODO conditional expressions and loops
	//if, else if, for, switch, case, while
	return type;
}
