#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strsubstr(char *str , int from, int count);

int strindexof(char *str, char chr);
int strlastindexof(char *str, char chr);

char *strconcat(char *str1, char *str2);
char *strjoin(char **strings, unsigned int count, char *join);

#endif /* STRINGUTILS_H_ */
