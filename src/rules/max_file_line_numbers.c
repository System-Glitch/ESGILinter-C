#include "rules/max_line_numbers.h"
#include "arraylist.h"
#include "stringutils.h"
#include "display.h"

unsigned int max_file_line_numbers(arraylist_t *file, unsigned int n, char *filename){

  if(file->size > n){
    char size_string[12];
    char n_string[12];
    sprintf(size_string, "%ld", file->size);
    sprintf(n_string, "%d", n);

    char *tmp0 = strconcat("Too many lines, actual: ", size_string);
    char *tmp1 = strconcat(tmp0, ", max: ");
    free(tmp0);
    tmp0 = strconcat(tmp1, n_string);

    print_warning(filename, n, NULL, tmp0);

    free(tmp1);
    free(tmp0);
    return 1;
  }
  return 0;
}
