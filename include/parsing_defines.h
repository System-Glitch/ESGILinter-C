#ifndef PARSING_DEFINES_H
#define PARSING_DEFINES_H

#include "scopetree.h"

typedef struct define {
	char   *name;
	char   *value;
	type_t  type; //Needs to be identified
} define_t;

define_t *define_init(char *name, char *value);
define_t *get_define_from_string(char *define);

void define_free(define_t *define);

#endif
