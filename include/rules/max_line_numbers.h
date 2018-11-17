#ifndef MAX_LINE_NUMBERS_H
#define MAX_LINE_NUMBERS_H

#include "scopetree.h"

unsigned int max_line_numbers(arraylist_t *file, unsigned int n);
unsigned int max_line_check(char *line, unsigned int n, unsigned int line_index);

#endif
