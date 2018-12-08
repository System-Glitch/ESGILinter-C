#ifndef MAX_LINE_NUMBERS_H
#define MAX_LINE_NUMBERS_H

#include "scopetree.h"

unsigned int check_max_line_length(arraylist_t *file, unsigned int n);
unsigned int line_length_check(char *line, unsigned int n, unsigned int line_index);

#endif
