#ifndef PARSING_TYPE_H
#define PARSING_TYPE_H

#include "stringutils.h"

typedef struct type {
	char          *name;
	unsigned char  is_pointer;
} type_t;

match_t *parse_type(char *line);

#endif
