#include "parsing_variables.h"

#define SKIP_WHITESPACES while(is_whitespace(c = line[index]) && index < length) { index++; }; \
						if(index >= length) return NULL;

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