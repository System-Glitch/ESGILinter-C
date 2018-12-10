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
  int index_parenthesis = strlastindexof(tmp, ')'); // can be null in do{ case
  int index_bracket = strlastindexof(tmp, '{');
  int index_do;

  if(index_bracket != -1 && index_line_break != -1 && !check_quotes(tmp, tmp + index_bracket, strlen(tmp))){
    index_do = check_do(tmp, index_bracket);
    if(index_do != -1){
      for(int i = index_do + 1; i < index_bracket; i++){
        if(tmp[i] == '\n'){
          print_warning(line->source, line->start_real_line, line->line, "Bracket is not end of line");
          free(tmp);
          return 1;
        }
      }
    }else if(index_parenthesis != -1 && index_line_break > index_parenthesis && index_line_break < index_bracket){
      print_warning(line->source, line->start_real_line, line->line, "Bracket is not end of line");
      free(tmp);
      return 1;
    }else{
      free(tmp);
      return 0;
    }
  }else{
    free(tmp);
    return 0;
  }
  free(tmp);
  return 0;
}

unsigned int check_do(char *line, int index_bracket){
  if(index_bracket == -1){
    return -1;
  }
  for(int i = index_bracket - 1; i > 0; i--){
    if(line[i] == 'o' && line[i-1] == 'd'){
      return i;
    }else if(!is_whitespace(line[i])){
      return -1;
    }
  }
  return -1;
}
