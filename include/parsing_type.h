#ifndef PARSING_TYPE_H
#define PARSING_TYPE_H

#include "stringutils.h"
#include "scopetree.h"

match_t *parse_type(char *line);
type_t   get_expression_type(char *line, scope_t *scope, char **undefined_variable, char **undefined_function);


#endif
