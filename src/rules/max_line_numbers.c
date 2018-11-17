#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "rules/max_line_numbers.h"
#include "arraylist.h"
#include "display.h"

//returns number of warnings
unsigned int max_line_numbers(arraylist_t *file, unsigned int n){
  unsigned int counter = 0;
  for(unsigned int i = 0; i < file->size; i++){
    counter += max_line_check(arraylist_get(file, i), n, i);
  }
  return counter;
}

//returns 1 if excess character(s)
unsigned int max_line_check(char *line, unsigned int n, unsigned int line_index){
  unsigned int counter = 0;
  for(int i = 0; (int) strlen(line); i++){
    counter++;
  }
  if(counter > n){
    print_warning("fictive_file.c", line_index, line, "Excess of characters");
    return 1;
  }
  else{
    return 0;
  }
}
