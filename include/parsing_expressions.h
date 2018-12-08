#ifndef PARSING_EXPRESSIONS_H
#define PARSING_EXPRESSIONS_H

#include "stringutils.h"
#include "scopetree.h"
#include "messages.h"

type_t parse_expression(char *line, int line_index, scope_t *scope, messages_t *messages);

#endif
