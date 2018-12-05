#include "rules/no_trailing_spaces.h"
#include <string.h>
#include "stringutils.h"
#include "fileloader.h"

int no_trailing_spaces(arraylist_t *file){
  if(!file) return -1;
  unsigned int counter = 0;
  for(int i = 0; i < file-> size; i++){
    if()
  }
  return counter;
}

int no_trailing_spaces_check(line_t *line){
  if(!line) return 0;
  int index;
  index = strlastindexof(line->line, '\n');
  if(index >= 1 && line->line[index-1] == ' '){
        print_warning(line->source, line->real_line, line->line, "Trailing space(s) before EOL");
        return 1;
  }
  return 0;
}
