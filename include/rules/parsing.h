#ifndef RULES_PARSING_H
#define RULES_PARSING_H

#include "scopetree.h"
#include "arraylist.h"

#define WRONG_TYPE_ASSIGNMENT 0
#define WRONG_TYPE_RETURN     1
#define WRONG_TYPE_TERNARY    2

unsigned int parse_and_check(scope_t *root_scope, arraylist_t *file, arraylist_t *functions, arraylist_t *variables, arraylist_t *conf);

#endif
