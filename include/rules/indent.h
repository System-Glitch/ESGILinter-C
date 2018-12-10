#ifndef INDENT_H_
#define INDENT_H_

#include <string.h>
#include "stringutils.h"
#include "arraylist.h"
#include "display.h"
#include "fileloader.h"

unsigned int indent(arraylist_t *file, int indent, char *filename);

void check_expression(char *line, const char *expr, int *adding, int *special);

int is_expression(char *line, char *tmp);

#endif /* INDENT_H_ */