#include "stringutils.h"

char *strsubstr(char *str , int from, int count) {
	char *result;
	if(str == NULL) return NULL;

	result = malloc((count+1) * sizeof(char));

	if(result == NULL) return NULL;

	strncpy(result, str+from, count);
	result[count] = '\0';
	return result;
}

int strindexof(char *str, char chr) {

	unsigned int length = strlen(str);

	for(unsigned int i = 0 ; i < length ; i++) {
		if(str[i] == chr)
			return i;
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
