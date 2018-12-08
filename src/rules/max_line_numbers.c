#include <string.h>
#include "rules/max_line_numbers.h"
#include "arraylist.h"
#include "stringutils.h"
#include "display.h"

//returns 1 if excess character(s)
static unsigned int line_length_check(char *line, unsigned int n, unsigned int line_index, char *filename) {
	char *tmp0 = NULL;
	char *tmp1 = NULL;
	char *display = NULL;
	char length_str[20];
	char max_str[12];
	size_t length = strlen(line);

	if(length >= 1 && line[length - 1] == '\n')
		length--;

	if(length > n){
		sprintf(length_str, "%ld", length);
		sprintf(max_str, "%d", n);
		tmp0 = strconcat("Too many characters, actual: ", length_str);
		tmp1 = strconcat(tmp0, ", max: ");
		free(tmp0);
		tmp0 = strconcat(tmp1, max_str);
		free(tmp1);
		display = trim(line);
		print_warning(filename, line_index + 1, display, tmp0);
		free(tmp0);
		free(display);
		return 1;
	} else {
		return 0;
	}
}

//returns number of warnings
unsigned int check_max_line_length(arraylist_t *file, unsigned int n, char *filename){
	unsigned int counter = 0;

	for(unsigned int i = 0; i < file->size; i++){
		counter += line_length_check(arraylist_get(file, i), n, i, filename);
	}
	return counter;
}
