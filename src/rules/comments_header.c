#include "rules/comments_header.h"
#include "stringutils.h"
#include "display.h"

unsigned int check_comments_header(arraylist_t *file, char *filename) {
	int index = 0;
	int length;
	char *line;

	for(size_t i = 0 ; i < file->size ; i++){
		line = arraylist_get(file, i);
		length = strlen(line);
		index = 0;
		while(is_whitespace(line[index]) && index < length) {
			index++;
		}
		if(index < length - 1) {
			if(line[index] == '/' && line[index + 1] == '*') {
				return 0;
			} else {
				print_warning(filename, i + 1, NULL, "File doesn't start with a comments header");
				return 1;
			}
		}
	}
	print_warning(filename, file->size, NULL, "File doesn't start with a comments header");
	return 1;
}

