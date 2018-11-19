#include <string.h>
#include "rules/max_line_numbers.h"
#include "arraylist.h"
#include "display.h"

unsigned int max_file_line_numbers(arraylist_t *file, unsigned int n){
  if(file->size > n){
    print_warning("fictive_file.c", n, arraylist_get(file, n), "Too much lines");
    return 1;
  }
  return 0;
}
