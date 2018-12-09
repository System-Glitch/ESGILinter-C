//
// Created by Pierre Delmer on 23/10/2018.
//
#include <errno.h>
#include "stringutils.h"
#include "scopetree.h"
#include "fileloader.h"
#include "display.h"

/*
 * Get the number of rows of the file
 * @params FILE
 * @return unsigned long
 */

unsigned int file_row_number(FILE *src)
{
    if(!src) return 0;
    char *line;
    unsigned int rows;

    rows = 0;

    /*
     * Memory allocation
     */
    line = malloc(sizeof(char)*1048);

    /*
     * Rewind the file
     */
    rewind(src);

    /*
     * List all of the line of the file
     */

    while(fgets(line, 1048, src) != NULL){
        rows++;
    }

    /*
     * Rewind the file
     */

    rewind(src);

    /*
     * Free the memory
     */
    free(line);
    return rows;
}

/*
 * Load a file in memory
 * @params char*
 * @return arrayList*
 */

void file_loader(arraylist_t *e, arraylist_t *files, arraylist_t *real_file, char *filename){

    if(strlen(filename) <= 0) return;
    unsigned int length;
    unsigned int i;
    unsigned int j;
    unsigned int index;
    int tempo;
    int init;
    char *line;
    char *tmp = NULL;
    char *tmp2 = NULL;
    char *tmp_line;
    char *res;
    char *for_loop = NULL;
    char *real;
    char c;
    int real_line;
    line_t *l;
    int line_counter;
    int counter;
    int start_for;
    int literal;
    int include_fold;
    size_t start_buffer;
    size_t file_length;
    size_t for_loop_length;
    int start_array;
    int array_length;
    FILE *src;

    src = fopen(filename,"rb");

    if(src == NULL){
        return;
    }

    length = file_row_number(src);
    /*
     * Memory allocation
     */

    line = malloc(sizeof(char) * 1048);
    tmp_line = malloc(sizeof(char) * 1048);
    tmp_line[0] = '\0';

    /*
     * Load the array
     */

    line_counter = 0;
    real_line = 0;
    start_for = 0;
    literal = 0;
    start_array = 0;
    array_length = 0;
    start_buffer = 0;
    include_fold = 0;

    for(i = 0; i < length; i++){
        tempo = 0;
        counter = 0;
        init = 0;
        line_counter++;
        if(!fgets(line, 1048, src)) {
            printf("%s[ERROR]%s %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
            exit(EXIT_FAILURE);
        }

        if(strstr(line, "#include \"") == line){
            int first_index = strindexof(line, '"');
            int last_index = strlastindexof(line, '"');
            tmp2 = strsubstr(line, first_index+1, last_index-first_index-1);
            last_index = strlastindexof(tmp2, '/');
            if(last_index != -1){
                tmp = strsubstr(tmp2, last_index+1, strlen(tmp2) - last_index);
                include_fold = 1;
                free(tmp2);
                tmp2 = NULL;
            }
            for(size_t k = 0; k < files->size; k++){
                if(include_fold){
                    if(strstr(files->array[k], tmp) != NULL){
                        file_loader(e, files, real_file, files->array[k]);
                        start_buffer = real_file->size;
                    }
                }else{
                    if(strstr(files->array[k], tmp2) != NULL){
                        file_loader(e, files, real_file, files->array[k]);
                        start_buffer = real_file->size;
                    }
                }
            }
            if(tmp != NULL) {
                free(tmp);
                tmp = NULL;
            }else{
                free(tmp2);
                tmp2 = NULL;
            }
            continue;
        }

        real = malloc(sizeof(char) * 1048);
        strcpy(real, line);
        arraylist_add(real_file, real);

        for_loop = strstr(line, "for");
        if(for_loop != NULL){
            for_loop += 3;
            index = 0;
            start_for = 1;
            for_loop_length = strlen(for_loop);
            while(is_whitespace(c = for_loop[index]) && index < for_loop_length) { index++; };
            if(for_loop[index] == '(') start_for = 1;
        }

        file_length = strlen(line);
        for(j = 0; j < file_length ; j++){
            if(line[j] == '"' && literal == 1){
                literal = 0;
            }else if(line[j] == '"' && literal == 0){
                literal = 1;
            }
            if(line[j] == '=' && start_array == 0){
                index = j+1;
                while(is_whitespace(c = line[index]) && index < file_length) { index++; };
                if(line[index] == '{'){
                    start_array = 1;
                    continue;
                }
            }
            if(start_array == 1 && line[j] == '{'){
                array_length++;
                continue;
            }
            if(start_array == 1 && line[j] == '}'){
                array_length--;
                if(array_length == 0) start_array = 0;
                continue;
            }

            if(((line[j] == ';' && start_for == 0) || (line[j] == '{' && start_array == 0) || (line[j] == '}' && start_array == 0)) && literal != 1 ){
                l = malloc(sizeof(line_t));
                l->source = strduplicate(filename);
                l->start_line_in_buffer = start_buffer+real_line;
                l->start_real_line = real_line;
                if(line[j+1] == '\n'){
                    real_line = i + 1;
                }
                if(strlen(tmp_line) != 0){
                    l->line = strduplicate(tmp_line);
                    tmp = strsubstr(line, tempo, j+1);
                    tmp2 = strconcat(l->line, tmp);
                    free(l->line);
                    free(tmp);
                    l->line = tmp2;
                    strcpy(tmp_line,"");
                }else{
                    if(!init){
                        l->line = strsubstr(line, tempo, j+1);
                    }else{
                        l->line = strsubstr(line, tempo, j-init);
                    }
                }
                arraylist_add(e, l);
                tempo = j+1;
                counter++;
                init = j;
                if(start_for == 1 && line[j] == '{')
                    start_for = 0;
            }
        }
        if(!counter){
            strcat(tmp_line,line);
        }else if(counter && tempo){
            res = strsubstr(line, tempo, strlen(line));
            strcat(tmp_line, res);
            free(res);
        }
    }


    /*
     * Free the memory
     */
    fclose(src);
    free(line);
    free(tmp_line);
}


/**
 * Search all the current files
 * @param conf
 * @param files
 */
void search_files(arraylist_t *conf, arraylist_t *files, char *path){
    if(!conf) return;

    struct dirent *read;
    DIR *rep;
    char *ext = NULL;
    char *name;

    rep = opendir(path);
    while ((read = readdir(rep))) {
        if(read->d_type == 8){
            ext = strsubstr(read->d_name, strlen(read->d_name)-2,2);
            if((strcmp(ext,".c") == 0 || strcmp(ext,".h") == 0) && is_excluded(conf, read->d_name) == 0){
                name = malloc(sizeof(char) * 1048);
                if(strcmp(path, ".") == 0){
                    strcpy(name,read->d_name);
                }else{
                    strcpy(name, path);
                    strcat(name,"/");
                    strcat(name, read->d_name);
                }
                arraylist_add(files, name);
            }
            free(ext);
            ext = NULL;
        }else if(read->d_type == 4 && strcmp(read->d_name,".") != 0 && strcmp(read->d_name,"..") != 0 && is_recursive(conf) != 0){

            if(strcmp(path, ".") == 0){
                strcpy(path, read->d_name);
            }else{
                strcat(path, "/");
                strcat(path, read->d_name);
            }
            search_files(conf, files, path);
        }
    }
    if(strindexof(path, '/') != -1){
        ext = strsubstr(path, 0, strlastindexof(path, '/'));
        strcpy(path, ext);
        free(ext);

    }else{
        strcpy(path,".");
    }
    closedir(rep);
}

/**
 * Return the current struct line of the file
 * @param file
 * @param index
 * @return line_t*
 */
line_t *get_line(arraylist_t *file, int index){
    if(!file) return NULL;
    if(index >= 0) return ((line_t*)(arraylist_get(file, (unsigned int)index)));
    return NULL;
}

/**
 * Free the buffer
 * @param buffer
 */

void free_buffer(arraylist_t *buffer) {
    line_t *line = NULL;

    if(!buffer) return;

    for(size_t i = 0; i < buffer->size; i++) {
        line = get_line(buffer, i);
        free(line->line);
        free(line->source);
        free(line);
    }
    arraylist_free(buffer, 0);
}