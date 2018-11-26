#ifndef PARSING_EXPRESSIONS_H
#define PARSING_EXPRESSIONS_H

#include "stringutils.h"
#include "scopetree.h"

type_t   parse_expression(char *line, int line_index, scope_t *scope, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params, arraylist_t *variables_list, arraylist_t *functions_list);

#endif
