#ifndef PARSING_OPERATIONS_H
#define PARSING_OPERATIONS_H

#include "stringutils.h"
#include "parsing_expressions.h"

type_t parse_operation(char *line, int line_index, scope_t *scope, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params);

#endif
