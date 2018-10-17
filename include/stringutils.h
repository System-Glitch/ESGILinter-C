#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

char *strsubstr(char *str , int from, int count);

unsigned int strcount(char *str, char chr);
unsigned int strcountuntil(char *str, char chr, char reverse);

int strindexof(char *str, char chr);
int strlastindexof(char *str, char chr);

char *strconcat(char *str1, char *str2);
char *strjoin(char **strings, unsigned int count, char *join);

char  exec_regex(regex_t * regex, char* regexp, char * source, int max_groups, regmatch_t (*pmatch)[]);
char *substr_regex_match(char *source, regmatch_t match);

#endif /* STRINGUTILS_H_ */
