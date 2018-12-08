#ifndef PARSING_OPERATIONS_H
#define PARSING_OPERATIONS_H

#include "stringutils.h"
#include "parsing_expressions.h"
#include "messages.h"

type_t parse_operation(char *line, int line_index, scope_t *scope, messages_t *messages);

#endif
