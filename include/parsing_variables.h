#ifndef PARSING_VARIABLES_H
#define PARSING_VARIABLES_H

#include "arraylist.h"
#include "stringutils.h"

arraylist_t *get_variables_from_declaration(int line_index, char *line);

#endif
