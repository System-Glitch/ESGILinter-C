#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strsubstr(char *str , int from, int count);

size_t strindexof(char *str, char chr);
int strlastindexof(char *str, char chr);

char *strconcat(char *str1, char *str2);
char *strjoin(char **strings, unsigned int count, char *join);


void strformat(char *str, int length);


char *substr_match(char *source, match_t match);


char is_whitespace(char c);
char is_alphanumeric(char c);


match_t *match_init();

#endif /* STRINGUTILS_H_ */
