#include "rules/array_bracket_eol.h"

unsigned int array_bracket_eol(arraylist_t *file){
  unsigned int counter = 0;

	for(unsigned int i = 0; i < file->size; i++){
		counter += array_bracket_eol_check(get_line(file, i));
	}
	return counter;
}

unsigned int array_bracket_eol_check(line_t *line){
  char *tmp = str_remove_comments(line->line);
  int index_line_break = strlastindexof(tmp, '\n');
  int index_parenthesis = strlastindexof(tmp, ')');
  int index_bracket = strlastindexof(tmp, '{');
  if(index_line_break > index_parenthesis && index_line_break < index_bracket && index_parenthesis != -1 && index_bracket != -1 && index_line_break != -1 && !check_quotes(tmp, tmp + index_bracket, strlen(tmp))) {
    print_warning(line->source, line->start_real_line, line->line, "bracket is not end of line");
    free(tmp);
    return 1;
  }
  free(tmp);
  return 0;
}
