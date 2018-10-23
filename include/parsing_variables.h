#ifndef PARSING_VARIABLES_H
#define PARSING_VARIABLES_H

#include <stdlib.h>
#include "stringutils.h"

unsigned char type_exists(char *type);

match_t *pvar_type(char *line);
match_t *pvar_name_and_value(char *names);
match_t *pvar_name(char *name, unsigned int *start_index, unsigned int *array_count);

#endif
