#include "rules/comma_spacing.h"


unsigned int comma_spacing(arraylist_t *buffer){
    char *line = NULL;
    size_t length;
    unsigned int counter = 0;
    line_t *e;
    for(int i = 0; i < buffer->size; i++){
        e = get_line(buffer, i);
        line = str_remove_comments(e->line);
        length = strlen(line);
        for(size_t j = 0; j < length; j++){
            if(line[j] == ',' && !check_quotes(line, line+j, (int)length) && line[j+1] != ' '){
                counter++;
                print_warning(e->source, e->start_real_line, e->line, "No space after a comma");
            }
        }
        free(line);
    }
    return counter;
}