//
// Created by Pierre Delmer on 23/10/2018.
//

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
    int tempo;
    int init;
    char *line;
    char *tmp;
    char *res;
    char *for_loop;
    char *real;
    char c;
    int index;
    int real_line;
    line_t *l;
    int line_counter;
    int counter;
    int start_for;
    int literal;
    int file_length;
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
    tmp = malloc(sizeof(char) * 1048);
    res = malloc(sizeof(char) * 1048);
    strcpy(tmp, "");

    /*
     * Load the array
     */

    line_counter = 0;
    real_line = 0;
    start_for = 0;
    literal = 0;
    start_array = 0;
    array_length = 0;
    for(i = 0; i < length; i++){
        tempo = 0;
        counter = 0;
        init = 0;
        line_counter++;
        fgets(line, 1048, src);
        real = malloc(sizeof(char) * 1048);
        strcpy(real, line);
        arraylist_add(real_line, real);
        real_line++;
        if(strstr(line, "#include \"") == line){
            int first_index = strindexof(line, '"');
            int last_index = strlastindexof(line, '"');
            tmp = strsubstr(line, first_index+1, last_index-first_index-1);
            last_index = strlastindexof(tmp, '/');
            if(last_index != -1){
                tmp = strsubstr(tmp, last_index+1, strlen(tmp) - last_index);
            }
            for(int k = 0; k < files->size; k++){
                if(strstr(files->array[k], tmp) != NULL){
                    file_loader(e, files, files->array[k]);
                }
            }
            strcpy(tmp,"");
            continue;
        }

        for_loop = strstr(line, "for");
        if(for_loop != NULL){
            for_loop += 3;
            index = 0;
            while(is_whitespace(c = for_loop[index]) && index < strlen(for_loop)) { index++; };
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
                while(is_whitespace(c = line[index]) && index < strlen(line)) { index++; };
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
                l->source = malloc(sizeof(char) * 255);
                l->line = malloc(sizeof(char) * 1048);
                strcpy(l->source, filename);
                l->start_line = i-line_counter;
                if(l->start_line < 0){
                    l->start_line = 1;
                }
                l->end_line = i+1;
                l->real_line = real_line;
                if(strlen(tmp) != 0){
                    strcpy(l->line, tmp);
                    tmp = strsubstr(line, tempo, j+1);
                    strcat(l->line, tmp);
                }else{
                    if(!init){
                        tmp = strsubstr(line, tempo, j+1);
                        strcpy(l->line, tmp);
                    }else{
                        tmp = strsubstr(line, tempo, j-init);
                        strcpy(l->line, tmp);
                    }
                }
                strcpy(tmp,"");
                arraylist_add(e, l);
                tempo = j+1;
                counter++;
                init = j;
                if(start_for == 1 && line[j] == '{')
                    start_for = 0;
            }
        }
        if(!counter){
            strcat(tmp,line);
        }else if(counter && tempo){
            res = strsubstr(line, tempo, strlen(line));
            strcat(tmp, res);
        }

        line = malloc(sizeof(char) * 1048);
    }



    /*
     * Free the memory
     */
    fclose(src);
    if(for_loop)
        free(for_loop);
    free(line);
    free(tmp);
    free(res);
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
    char *ext;
    char *name;

    ext = malloc(sizeof(char) * 255);



    rep = opendir(path);
    while ((read = readdir(rep))) {
        if(read->d_type == 8){
            strcpy(ext, strsubstr(read->d_name, strlen(read->d_name)-2,2));
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
        strcpy(path, strsubstr(path, 0, strlastindexof(path, '/')));
    }else{
        strcpy(path,".");
    }

    closedir(rep);

    free(ext);
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