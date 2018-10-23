#include "parsing_variables.h"

#define SKIP_WHITESPACES while(is_whitespace(c = line[index]) && index < length) { index++; };

unsigned char type_exists(char *type) {
	static const char *known_types[] = {"char","short","int","unsigned","long","signed","float","double","size_t"};
	for(unsigned i = 0 ; i < 9 ; i++)
		if(!strcmp(type, known_types[i]))
			return 1;

	return 0;
}

match_t *pvar_type(char *line) {
	unsigned int index = 0;
	unsigned int type_start = 0;
	unsigned int type_end = 0;

	unsigned int type_word_start = 0;
	unsigned int type_word_end = 0;

	char *word;

	size_t length = strlen(line);
	unsigned char c;
	match_t *match = NULL;

	if(length < 3) return NULL; //3 is the minimum size for a declaration

	SKIP_WHITESPACES

	if(strstr(line + index, "static") == line + index)
		index += 6;

	SKIP_WHITESPACES

	if(strstr(line + index, "const") == line + index)
		index += 5;

	SKIP_WHITESPACES

	type_start = index;

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
	} while(1);

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

	match = match_init();
	if(match != NULL) {
		match->index_start = type_start;
		match->index_end = type_end;
	}

	return match;
}

match_t *pvar_name(char *names, unsigned int *start_index, unsigned int *array_count) {

	unsigned int index = 0;
	unsigned char in_content = 0;
	unsigned char expect_comma = 0;
	unsigned int nested = 0;
	unsigned char value_start_index = 0;

	char *line = names + *start_index;
	unsigned int length = strlen(line);
	unsigned char c;
	match_t *match = NULL;

	if(*start_index > strlen(names)) return NULL;

	match = match_init();

	if(match == NULL) return NULL;

	SKIP_WHITESPACES

	match->index_start = index;

	//Until not alphanumeric
	while((is_alphanumeric(c = line[index]) || c == '*') && index < length) {
		index++;
	}

	match->index_end = index;

	if(c != ',') {

		SKIP_WHITESPACES

		//Is array?
		do {
			if(index < length && line[index] == '[') {
				index++;
				(*array_count)++;

				//Any character until closing bracket
				while((c = line[index]) != ']' && index < length) {
					index++;
				}
				index++;

				SKIP_WHITESPACES
			} else break;
		} while(index < length);

		SKIP_WHITESPACES

		//Equal symbol?
		if(index < length && line[index++] == '=') {
			
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

			if(in_content > 0 || nested > 0 || index - 1 == value_start_index) { //Invalid syntax or missing value before comma
				free(match);
				match = NULL;
			}
		}
	} else {
		index++;
	}

	*start_index += index;

	return match;
}
