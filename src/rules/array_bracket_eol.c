#include "rules/array_bracket_eol.h"
#include <string.h>
#include "stringutils.h"
#include "fileloader.h"
#include "display.h"

unsigned int array_bracket_eol(arraylist_t *file){
    unsigned int counter;
    if(!file) return 0;
    counter = 0;
    for(int i = 0 ; i < file->size ; i++){
        if(array_bracket_eol_check(get_line(file, i))) counter++;
    }
    return counter;
}


int array_bracket_eol_check(line_t *line){
    if(!line) return 0;
    int index;
    index = strlastindexof(line->line, '{');
    if(index >= 1 && line->line[index-1] == '\n'){
        print_warning(line->source, line->real_line, line->line, "Bracket after a line brake");
        return 1;
    }
    return 0;
}