#include "stringutils.h"

char *str_remove_comments(char *str) {
	char *result = NULL;
	char *tmp = NULL;
	int comment_start = -1;
	int comment_end   = -1;
	size_t length = strlen(str);

	for(unsigned i = 0 ; i < length ; i++) {
		if(comment_start == -1) {
			if(str[i] == '/' && str[i+1] == '*') {
				comment_start = i;
				i++;
			}
		} else {
			if(str[i] == '*' && str[i+1] == '/') {
				comment_end = i + 2;
				break;
			}
		}
	}

	if(comment_start != -1 && comment_end != -1) {
		result = malloc((length + 1) * sizeof(char));
		strncpy(result, str, comment_start);
		strncpy(result + comment_start, str + comment_end, length - comment_end);
		result[comment_start + (length - comment_end)] = '\0';
		tmp = str_remove_comments(result);
		free(result);
		result = tmp;
	} else {
		result = strduplicate(str);
	}

	return result;
}

char *strduplicate(char *str) {
	char *result = NULL;
	int len;

	if(str == NULL) return NULL;

	len = strlen(str);
	result = malloc((len + 1) * sizeof(char));

	if(result == NULL) return NULL;

	strcpy(result, str);
	result[len] = '\0';

	return result;
}

char *strsubstr(char *str , int from, int count) {
	char *result;
	if(str == NULL) return NULL;

	result = malloc((count+1) * sizeof(char));

	if(result == NULL) return NULL;

	strncpy(result, str+from, count);
	result[count] = '\0';
	return result;
}


unsigned int strcount(char *str, char chr) {

	unsigned int length = strlen(str);
	unsigned int count = 0;

	for(unsigned int i = 0 ; i < length ; i++) {
		if(str[i] == chr)
			count++;
	}
	return count;
}

unsigned int strcountuntil(char *str, char chr, char reverse, char ignore_whitespace) {

	unsigned int length = strlen(str);
	unsigned int count = 0;

	if(!reverse) {
		for(unsigned int i = 0 ; i < length ; i++) {
			if(str[i] == chr || (ignore_whitespace && is_whitespace(str[i])))
				count++;
			else
				break;
		}
	} else {
		for(int i = length - 1 ; i >= 0 ; i--) {
			if(str[i] == chr || (ignore_whitespace && is_whitespace(str[i])))
				count++;
			else
				break;
		}
	}
	return count;
}

int strindexof(char *str, char chr) {

	size_t length = strlen(str);

	for(size_t i = 0 ; i < length ; i++) {
		if(str[i] == chr)
			return (int)i;
	}
	return -1;
}

int strlastindexof(char *str, char chr) {

	int length = strlen(str);

	for(unsigned int i = length-1 ; i > 0 ; i--) {
		if(str[i] == chr)
			return i;
	}
	return -1;
}

char *strconcat(char *str1, char *str2) {
	char *result;
	unsigned int length = 0;

	if(str1 != NULL) length += strlen(str1);
	if(str2 != NULL) length += strlen(str2);

	result = malloc((length + 1) * sizeof(char));

	if(result != NULL) {

		result[0] = '\0';
		result[length - 1] = '\0';

		if(str1 != NULL) strcat(result, str1);
		if(str2 != NULL) strcat(result, str2);

	}

	return result;
}

char *strjoin(char **strings, unsigned int count, char *join) {
	char *result;
	unsigned int length = 0;

	if(strings == NULL || join == NULL) return NULL;

	//Calculate length
	for(unsigned int i = 0 ; i < count ; i++)
		if(strings[i] != NULL)
			length += strlen(strings[i]);

	length += (count-1) * strlen(join); //Join characters

	result = malloc((length + 1) * sizeof(char));

	if(result != NULL) {

		result[0] = '\0';
		result[length - 1] = '\0';

		//Concat
		for(unsigned int i = 0 ; i < count ; i++) {
			strcat(result, strings[i]);
			if(i < count - 1)
				strcat(result, join);
		}
	}

	return result;
}


void strformat(char *str, size_t length){
    if(strlen(str) <= 0 || length <= 0) return;

    if(strlen(str) <= length){
        str[strlen(str) - 1] = '\0';
    }
}

char *substr_match(char *source, match_t match) {
	int length = match.index_end - match.index_start;
	char *substr = malloc((length + 1) * sizeof(char));

	if(substr != NULL) {
		strncpy(substr, source + match.index_start, length);
		substr[length] = '\0';
	}

	return substr;
}


char is_whitespace(char c) {
	return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\f';
}


char is_alphanumeric(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
}


match_t *match_init() {
	match_t *match = malloc(sizeof(match_t));
	return match;
}
