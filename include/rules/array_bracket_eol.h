#ifndef ARRAY_BRACKET_EOL_H
#define ARRAY_BRACKET_EOL_H

#include <string.h>
#include "arraylist.h"
#include "stringutils.h"
#include "display.h"
#include "fileloader.h"

unsigned int array_bracket_eol(arraylist_t *file);

unsigned int array_bracket_eol_check(line_t *line);

unsigned int check_do(char *line, int index_bracket);

#endif
