#include <string.h>
#include "rules/max_line_numbers.h"
#include "arraylist.h"
#include "display.h"

//returns number of warnings
unsigned int check_max_line_length(arraylist_t *file, unsigned int n){
  unsigned int counter = 0;

  for(unsigned int i = 0; i < file->size; i++){
    counter += max_line_check(arraylist_get(file, i), n, i);
  }
  return counter;
}

//returns 1 if excess character(s)
unsigned int line_length_check(char *line, unsigned int n, unsigned int line_index){
  if(strlen(line) > n){
    print_warning("fictive_file.c", line_index, line, "Excess of characters");
    return 1;
  }
  else{
    return 0;
  }
}
