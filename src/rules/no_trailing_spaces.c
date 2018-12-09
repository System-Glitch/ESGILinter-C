#include "rules/no_trailing_spaces.h"
#include "stringutils.h"
#include "display.h"

static int no_trailing_spaces_check(char *line, int line_index, char *filename) {
	int index;
	index = strlastindexof(line, '\n');
	if((index >= 1 && line[index-1] == ' ') || (index >= 2 && line[index - 1] == '\r' && line[index - 2] == ' ')){
		print_warning(filename, line_index + 1, line, "Trailing space(s) before EOL"); //TODO trim
		return 1;
	}
	return 0;
}

int no_trailing_spaces(arraylist_t *file, char *filename) {
	if(!file) return -1;
	unsigned int counter = 0;
	for(size_t i = 0 ; i < file->size ; i++){
		counter += no_trailing_spaces_check(arraylist_get(file, i), i, filename);
	}
	return counter;
}

