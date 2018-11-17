#ifndef UNDECLARED_VARIABLE_H
#define UNDECLARED_VARIABLE_H

#include "scopetree.h"

unsigned int check_undeclared_variables(scope_t *root_scope, arraylist_t *file);

#endif