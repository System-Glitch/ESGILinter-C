#include <stdlib.h>
#include "parsing_type.h"
#include "scopetree.h"

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