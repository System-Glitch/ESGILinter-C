#include <stdlib.h>
#include "parsing_variables.h"
#include "parsing_type.h"
#include "scopetree.h"

static char parse_variable_value(char *line, size_t length, unsigned int *i) {
	unsigned int index = *i;
	unsigned char in_content = 0;
	unsigned char expect_comma = 0;
	unsigned int nested = 0;
	unsigned char value_start_index = 0;
	char c;

	SKIP_WHITESPACES

	//Expect value
	//Until comma (if outside quotes and braces)
	value_start_index = index;
	do {
		c = line[index];
		if(!expect_comma && !in_content && (c == '"' || c == '{')) {
			in_content = c;
			nested = 1;
		} else if(!expect_comma && in_content) {

			if(in_content == '{' && c == '{') {
				nested++;
			} else if(in_content == '{' && c == '}') {
				nested--;
				if(nested <= 0) {
					in_content = 0;
					expect_comma = 1;
				}
			} else if(in_content == '"' && c == '"' && line[index - 1] != '\\') {
				in_content = 0;
				nested = 0;
				expect_comma = 1;
			}

		} else if(c == ',') {
			index++;
			break;
		}
		index++;
	} while(index < length);

	*i = index;
	return !(in_content > 0 || nested > 0 || index - 1 == value_start_index); //Check for invalid syntax or missing value before comma
}

static unsigned int parse_variable_array(char *line, size_t length, unsigned int *i) {
	unsigned int index = *i;
	unsigned int array_count = 0;
	char c;

	//Is array?
	do {
		if(index < length && line[index] == '[') {
			index++;
			array_count++;

			//Any character until closing bracket
			while((c = line[index]) != ']' && index < length) {
				index++;
			}
			index++;

			SKIP_WHITESPACES
		} else break;
	} while(index < length);

	*i = index;
	return array_count;
}

match_t *parse_variable_name(char *names, unsigned int *start_index, unsigned int *array_count) {
	unsigned int index  = 0;
	char *line          = names + *start_index;
	unsigned int length = strlen(line);
	match_t *match      = NULL;
	unsigned char c;

	if(*start_index > strlen(names)) return NULL;

	match = match_init();

	if(match == NULL) return NULL;
	else if(line[index] == ';') {
		free(match);
		return NULL;
	}

	SKIP_WHITESPACES

	match->index_start = index;

	//Until not alphanumeric
	while((is_alphanumeric(c = line[index]) || c == '*') && index < length) {
		index++;
	}

	match->index_end = index;

	if(c != ';') {

		if(c != ',') {

			SKIP_WHITESPACES

			*array_count += parse_variable_array(line, length, &index);

			SKIP_WHITESPACES

			if(index < length) {
				//Equal symbol?
				if(line[index] == '=') {
					if(!parse_variable_value(line, length, &index)) { //Invalid syntax or missing value before comma
						free(match);
						match = NULL;
					}
				} else if(line[index] != ',' && line[index] != ';') { //Syntax error
					free(match);
					match = NULL;
				}
			}
		} else {
			index++;
		}
	}

	*start_index += index;

	return match;
}

static field_t *get_variable_from_declaration(int line_index, char *type, int star_count_type, char *declaration, unsigned int *names_index) {

	field_t *variable = NULL;
	match_t *match = NULL;
	unsigned int star_count, sub_index, tmp_index;
	char *tmp;
	char *name;
	unsigned int array_count = 0;

	tmp_index = *names_index;
	match = parse_variable_name(declaration, names_index, &array_count);

	if(match != NULL) {
		tmp = substr_match(declaration + tmp_index, *match);
		free(match);

		star_count = strcount(tmp, '*');
		sub_index  = strcountuntil(tmp, '*', 0, 1);

		//Remove stars
		name = strsubstr(tmp, sub_index, strlen(tmp) - star_count);
		free(tmp);

		if(name != NULL)
			variable = field_init(name, strduplicate(type), star_count + array_count + star_count_type, line_index);
	}

	return variable;
}

static match_t *match_for_loop(char *line) {
	match_t *match = NULL;
	unsigned int index = 0;
	size_t length = strlen(line);
	char c;

	SKIP_WHITESPACES

	if(strstr(line + index, "for") == line + index) {

		index += 3; // Skip "for"

		SKIP_WHITESPACES

		if(c != '(') return NULL; //Syntax error

		index++;
		SKIP_WHITESPACES

		match = match_init();
		if(match == NULL) return NULL;

		match->index_start = index;
		match->index_end = strindexof(line + index , ';') + index + 1;

		if(match->index_start > match->index_end) {
			free(match);
			match = NULL;
		}
	}

	return match;
}

arraylist_t *get_variables_from_declaration(int line_index, char *line) {
	unsigned int star_count_type;
	unsigned int type_sub_index;
	unsigned int length;
	unsigned int type_length;
	unsigned int names_index = 0;
	unsigned int type_start_index = 0;
	arraylist_t *list   = NULL;
	field_t *variable   = NULL;
	match_t *for_loop   = NULL;
	match_t *match_type = NULL;
	char *tmp_names;
	char *type;
	char *tmp_line = NULL;

	tmp_line = str_remove_comments(line);

	for_loop = match_for_loop(tmp_line);
	if(for_loop != NULL) {
		line = substr_match(tmp_line, *for_loop);
		free(tmp_line);
		free(for_loop);
	} else {
		line = tmp_line;
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
		tmp_names   = strsubstr(line, type_start_index + type_length, strlen(line) - type_length);
		length      = strlen(tmp_names);

		if(length > 0) {
			list            = arraylist_init(5);
			star_count_type = strcount(type, '*');
			type_sub_index  = strcountuntil(type, '*', 1, 1);

			//Remove stars
			type[type_length - type_sub_index] = '\0';

			if(strcount(type, '*')) { //Contains stars inside the type name -> invalid type
				free(tmp_names);
				free(type);
				arraylist_free(list, 1);
				free(line);
				return NULL;
			}

			do {
				variable = get_variable_from_declaration(line_index, type, star_count_type, tmp_names, &names_index);
				if(variable != NULL)
					arraylist_add(list, variable);
			} while(variable != NULL && names_index < length);

			if(list->size == 0) {
				arraylist_free(list, 1);
				list = NULL;
			}
		}

		free(type);
		free(tmp_names);
	}

	free(line);

	return list;
}
