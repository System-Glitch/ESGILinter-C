#include <stdlib.h>
#include "parsing_variables.h"
#include "scopetree.h"

#define SKIP_WHITESPACES while(is_whitespace(c = line[index]) && index < length) { index++; };

unsigned char type_exists(char *type) {
	static const char *known_types[] = {"char","short","int","unsigned","long","signed","float","double","size_t"};
	for(unsigned i = 0 ; i < 9 ; i++)
		if(!strcmp(type, known_types[i]))
			return 1;

	return 0;
}

static void pvar_type_word(char *line, size_t length, unsigned int *i) {
	char c;
	unsigned int index = *i;
	int type_word_start = 0;
	int type_word_end = 0;
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
		}
		free(word);
	} while(c != ';');

	*i = index;
}

static match_t *pvar_type(char *line) {
	unsigned int index = 0;
	int type_start = -1;
	int type_end = -1;

	size_t length = strlen(line);
	unsigned char c;
	match_t *match = NULL;

	if(length < 3) return NULL; //3 is the minimum size for a declaration

	SKIP_WHITESPACES

	if(strstr(line + index, "static") == line + index) //Skip static keyword
		index += 6;

	SKIP_WHITESPACES

	if(strstr(line + index, "const") == line + index) //Skip const keyword
		index += 5;

	SKIP_WHITESPACES

	type_start = index;

	pvar_type_word(line, length, &index);

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

static char pvar_value(char *line, size_t length, unsigned int *i) {
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

static unsigned int pvar_array(char *line, size_t length, unsigned int *i) {
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

static match_t *pvar_name(char *names, unsigned int *start_index, unsigned int *array_count) {

	unsigned int index  = 0;
	char *line          = names + *start_index;
	unsigned int length = strlen(line);
	match_t *match      = NULL;
	unsigned char c;

	if(*start_index > strlen(names)) return NULL;

	match = match_init();

	if(match == NULL || line[index] == ';') return NULL;

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

			*array_count += pvar_array(line, length, &index);

			SKIP_WHITESPACES

			//Equal symbol?
			if(index < length && line[index++] == '=') {
				if(!pvar_value(line, length, &index)) { //Invalid syntax or missing value before comma
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

static field_t *get_variable_from_declaration(char *type, int star_count_type, char *declaration, unsigned int *names_index) {

	field_t *variable = NULL;
	match_t *match = NULL;
	unsigned int star_count, sub_index, tmp_index;
	char *tmp;
	char *name;
	unsigned int array_count = 0;

	tmp_index = *names_index;
	match = pvar_name(declaration, names_index, &array_count);

	if(match != NULL) {
		tmp = substr_match(declaration + tmp_index, *match);
		free(match);

		star_count = strcount(tmp, '*');
		sub_index  = strcountuntil(tmp, '*', 0, 1);

		//Remove stars
		name = strsubstr(tmp, sub_index, strlen(tmp) - star_count);
		free(tmp);

		if(name != NULL)
			variable = field_init(name, strduplicate(type), star_count + array_count + star_count_type);
	}

	return variable;
}

arraylist_t *get_variables_from_declaration(char *line) {
	unsigned int star_count_type;
	unsigned int type_sub_index;
	unsigned int length;
	unsigned int type_length;
	unsigned int names_index = 0;
	arraylist_t *list = NULL;
	field_t *variable = NULL;
	char *tmp_names;
	char *type;

	match_t *match_type = pvar_type(line);
	if(match_type == NULL) {
		return NULL;
	}

	type = substr_match(line, *match_type);
	type_length = strlen(type);
	free(match_type);


	tmp_names = strsubstr(line, type_length, strlen(line) - type_length);
	length    = strlen(tmp_names);

	if(type != NULL && length > 0) {

		list            = arraylist_init(5);
		star_count_type = strcount(type, '*');
		type_sub_index  = strcountuntil(type, '*', 1, 1);

		//Remove stars
		type[strlen(type) - type_sub_index] = '\0';

		if(strcount(type, '*')) {
			free(tmp_names);
			free(type);
			arraylist_free(list, 1);
			return NULL;
		}

		do {
			variable = get_variable_from_declaration(type, star_count_type, tmp_names, &names_index);
			if(variable != NULL)
				arraylist_add(list, variable);
		} while(variable != NULL && names_index < length);

		free(type);


		if(list->size == 0) {
			arraylist_free(list, 1);
			list = NULL;
		}
	}

	free(tmp_names);

	return list;
}
