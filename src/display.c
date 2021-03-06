#include <stdio.h>
#include "display.h"

void print_warning(char *file, int line_index, char *line, const char *message) {
	print_message(file, line_index, line, message, "WARNING", COLOR_YELLOW);
}

void print_error(char *file, int line_index, char *line, const char *message) {
	print_message(file, line_index, line, message, "ERROR", COLOR_RED_BOLD);
}

void print_message(char *file, int line_index, char *line, const char *message, const char *type, const char *color) {
	if(line == NULL)
		printf("%s[%s]%s At %s%s:%d%s: %s\n", color, type, FORMAT_RESET, COLOR_CYAN_BOLD, file, line_index, FORMAT_RESET, message);
	else
		printf("%s[%s]%s At %s%s:%d%s: %s\n  %s\n", color, type, FORMAT_RESET, COLOR_CYAN_BOLD, file, line_index, FORMAT_RESET, message, line);
}
