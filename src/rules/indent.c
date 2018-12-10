#include "rules/indent.h"

unsigned int check_indent(arraylist_t *file, int indent, char *filename){
    if(!file) return 0;
    unsigned int counter = 0;
    int level = 0;
    int count = 0;
    int adding = 0;
    int special = 0;
    int line_counter = 0;
    int start_switch;
    int mem;
    int brackets;
    size_t j;
    size_t index;
    size_t length;
    size_t tmp_length;
    char c;
    char *tmp;
    char *line = NULL;
    char *trimmed = NULL;

    start_switch = 0;
    brackets = 0;
    mem = 0;

    for(size_t h = 0; h < file->size; h++){
        line = arraylist_get(file, h);
        line_counter++;
        length = strlen(line);
        if(line[0] == '\n' || (length >= 2 && line[0] == '\r' && line[1] == '\n')) continue;
        level += adding;
        adding = 0;
        j = 0;
        count = 0;
        while(line[j] == ' '){
            j++;
            count++;
        }
        for(j = 0; j < length; j++){
            if(line[j] == '{'){
                adding++;
                if(start_switch){
                    brackets++;
                }
            }else if(line[j] == '}'){
                level--;
                if(start_switch){
                    brackets--;
                }
            }
        }
        tmp = strstr(line, "case");
        if(tmp && is_expression(line, tmp)){
            level = mem+1;
        }
        if(brackets == 0 && start_switch == 1){
            start_switch = 0;
            level = mem;
        }
        if(count != (indent*level)){
            trimmed = trim(line);
            print_warning(filename, line_counter, trimmed, "Wrong indentation");
            free(trimmed);
            counter++;
        }
        if(special){
            level--;
            special = 0;
        }
        check_expression(line, "if", &adding, &special);
        check_expression(line, "for", &adding, &special);
        tmp = strstr(line,"case");
        index = 4;
        if(tmp && is_expression(line, tmp)){
            tmp_length = strlen(tmp);
            while(is_whitespace(c = tmp[index]) && index < tmp_length) { index++; };
            if(strchr(tmp, '{') == NULL)
                adding++;
        }

        check_expression(line, "while", &adding, &special);
        tmp = strstr(line, "switch");
        index = 6;
        if(tmp && is_expression(line, tmp)){
            tmp_length = strlen(tmp);
            while(is_whitespace(c = tmp[index]) && index < tmp_length) { index++; };
            if(tmp[index] == '('){
                mem = level;
                start_switch = 1;
                brackets++;
                adding++;
            }
        }
    }
    return counter;
}

void check_expression(char *line, const char *expr, int *adding, int *special){
    char *tmp = NULL;
    size_t tmp_length;
    size_t index;
    char c;

    tmp = strstr(line, expr);
    index = strlen(expr);
    if(tmp) {
        if(!is_expression(line, tmp)) return;
        tmp_length = strlen(tmp);
        while (is_whitespace(c = tmp[index]) && index < tmp_length) { index++; };
        if (*adding == 0 && tmp[index] == '(') {
            (*adding) += 1;
            (*special) = 1;
        }
    }
}


int is_expression(char *line, char *tmp){
    size_t index = 0;
    size_t length;
    char c;

    length = strlen(line);

    while (is_whitespace(c = line[index]) && index < length) { index++; };
    if(!strcmp(tmp, line+index)) return 1;
    return 0;

}