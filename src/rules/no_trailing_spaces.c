#include "rules/no_trailing_spaces.h"
#include <string.h>
#include "stringutils.h"
#include "fileloader.h"

int no_trailing_spaces(arraylist_t *file){
  if(!file) return -1;
  unsigned int counter = 0;
  for(int i = 0; i < file-> size; i++){
    counter += no_trailing_spaces_check(arraylist_get(file, i));
  }
  return counter;
}

int no_trailing_spaces_check(char *line){
  int index;
  index = strlastindexof(line, "\n");
  if(index >= 1 && line[index-1] == " "){
    print_warning(line->source, line->real_line, line->line, "Trailing space(s) before EOL");
    return 1;
  }
  else if(index >= 1 && line[index - 1] == "\r" && line[index - 2] == " "){
    print_warning(line->source, line->real_line, line->line, "Trailing space(s) before EOL");
    return 1;
  }
  return 0;
}
