#ifndef PARSING_VARIABLES_H
#define PARSING_VARIABLES_H

#include "arraylist.h"
#include "stringutils.h"

unsigned char type_exists(char *type);

arraylist_t *get_variables_from_declaration(char *line);

#endif
