#ifndef PARSING_FUNCTIONS_H
#define PARSING_FUNCTIONS_H

#include "stringutils.h"
#include "scopetree.h"

function_t *function_init(char *name, unsigned char is_prototype, char *type, unsigned char type_is_pointer, arraylist_t *params);
function_t *get_function_from_declaration(char *line);

void function_free(function_t *function);

#endif
