#ifndef PARSING_VARIABLES_H
#define PARSING_VARIABLES_H

#include "arraylist.h"
#include "stringutils.h"

arraylist_t *get_variables_from_declaration(int line_index, char *line);
match_t     *parse_variable_name(char *names, unsigned int *start_index, unsigned int *array_count);

#endif
