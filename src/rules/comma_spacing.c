#include "rules/comma_spacing.h"


unsigned int comma_spacing(arraylist_t *buffer, arraylist_t *real_file){
    char *line = NULL;
    size_t length;
    unsigned int counter = 0;
    line_t *e;
    char *display = NULL;
    unsigned int breaks = 0;

    for(size_t i = 0; i < buffer->size; i++){
        e = get_line(buffer, i);
        line = str_remove_comments(e->line);
        length = strlen(line);
        for(size_t j = 0; j < length; j++){
            if(line[j] == ',' && !check_quotes(line, line+j, (int)length) && line[j+1] != ' '){
                counter++;
                breaks = strcount_heading_line_breaks(e->line) + (e->start_real_line == 0 ? 1 : 0);
                display = trim(arraylist_get(real_file, e->start_real_line + breaks - 1));
                print_warning(e->source, e->start_real_line + breaks, display, "No space after a comma");
                free(display);
            }
        }
        free(line);
    }
    return counter;
}