#ifndef PARSING_VARIABLES_H
#define PARSING_VARIABLES_H

#include <stdlib.h>
#include "stringutils.h"

unsigned char type_exists(char *type);

match_t *pvar_type(char *line);

#endif
