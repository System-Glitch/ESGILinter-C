#ifndef PARSING_OPERATIONS_H
#define PARSING_OPERATIONS_H

#include "stringutils.h"
#include "parsing_expressions.h"
#include "messages.h"

//TODO clean params with a struct (or single arraylist containing a struct message)
type_t parse_operation(char *line, int line_index, scope_t *scope, messages_t *messages);

#endif
