#ifndef PARSING_TYPE_H
#define PARSING_TYPE_H

#include "stringutils.h"
#include "scopetree.h"

match_t *parse_type(char *line);
type_t   get_expression_type(char *line, int line_index, scope_t *scope, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params);


#endif
