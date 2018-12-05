#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct match {
	unsigned int index_start;
	unsigned int index_end;
} match_t;

char *str_remove_comments(char *str);
char *strduplicate(char *str);
char *strsubstr(char *str , int from, int count);

unsigned int strcount(char *str, char chr);
unsigned int strcountuntil(char *str, char chr, char reverse, char ignore_whitespace);

int strindexof(char *str, char chr);
int strlastindexof(char *str, char chr);

char *strconcat(char *str1, char *str2);
char *strjoin(char **strings, unsigned int count, char *join);


void strformat(char *str, size_t length);


char *substr_match(char *source, match_t match);


char is_whitespace(char c);
char is_alphanumeric(char c);
char is_digit(char c);
char is_line_break(char c);

unsigned char check_quotes(char *line, char *occurrence, int length);
unsigned char check_parenthesis(char *line, char *occurrence, int length);

char *remove_parenthesis(char *line, int length);
char *generate_char_sequence(char c, unsigned int count);

match_t *match_init();

#endif /* STRINGUTILS_H_ */
