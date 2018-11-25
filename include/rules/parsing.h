#ifndef RULES_PARSING_H
#define RULES_PARSING_H

#include "scopetree.h"

unsigned int parse_and_check(scope_t *root_scope, arraylist_t *file);

#endif