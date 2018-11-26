//
// Created by Pierre Delmer on 23/10/2018.
//

#include <stringutils.h>
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

arraylist_t* file_loader(char *filename){

    if(strlen(filename) <= 0) return NULL;

    arraylist_t *e;
    unsigned int length;
    unsigned int i;
    unsigned int j;
    int tempo;
    char *line;
    char *tmp;
    char *res;
    int initialized;
    int past_length;
    int counter;
    FILE *src;

    src = fopen(filename,"rb");

    if(src == NULL){
        return NULL;
    }

    length = file_row_number(src);
    /*
     * Memory allocation
     */
    e = arraylist_init(length);
    line = malloc(sizeof(char) * 1048);
    res = malloc(sizeof(char)*1048);

    /*
     * Load the array
     */
    initialized = 0;
    tmp = NULL;
    past_length = 0;
    for(i = 0; i < length; i++){

        tempo = 0;
        counter = 0;
        fgets(line, 1048, src);

        if(!initialized && i != 0){
            past_length = (int)strlen(tmp);
            strcat(tmp, line);
        }

        for(j = 0; j < strlen(line) ; j++){
            switch(line[j]){
                case ';':
                    if(!initialized && i != 0){
                        strcpy(res,strsubstr(tmp, tempo, past_length+1));
                    }else{
                        strcpy(res,strsubstr(line, tempo, j+1));
                    }
                    tempo = j+1;
                    past_length = 0;
                    arraylist_add(e, res);
                    initialized = 1;
                    counter = 1;
                    break;
                case '{':
                    if(!initialized && i != 0){
                        strcpy(res,strsubstr(tmp, tempo, past_length+1));
                    }else{
                        strcpy(res,strsubstr(line, tempo, j+1));
                    }
                    tempo = j+1;
                    past_length = 0;
                    arraylist_add(e, res);
                    initialized = 1;
                    counter = 1;
                    break;
                case '}':
                    if(!initialized && i != 0){
                        strcpy(res,strsubstr(tmp, tempo, past_length+1));
                    }else{
                        strcpy(res,strsubstr(line, tempo, j+1));
                    }
                    tempo = j+1;
                    past_length = 0;
                    arraylist_add(e, res);
                    initialized = 1;
                    counter = 1;
                    break;
            }
        }

        if(counter == 0){
            initialized = 0;
        }

        if(!initialized){
            tmp = malloc(sizeof(char) * 1048);
            strcpy(tmp,line);
        }
        line = malloc(sizeof(char) * 1048);
        res = malloc(sizeof(char)*1048);
    }



    /*
     * Free the memory
     */
    fclose(src);
    free(line);
    free(res);
    free(tmp);


    if(e->size){
        return e;
    }else{
        return NULL;
    }
}
