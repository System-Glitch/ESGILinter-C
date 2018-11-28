//
// Created by Pierre Delmer on 23/10/2018.
//

#ifndef ESGILINTER_C_FILELOADER_H
#define ESGILINTER_C_FILELOADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "arraylist.h"
#include "conf.h"

typedef struct line {
    char *line;
    char *source;
    int   start_line;
    int   end_line;
    int   real_line;
} line_t;

unsigned int file_row_number(FILE *src);

void file_loader(arraylist_t *e, arraylist_t *files, char *filename);

char* file_to_buffer(FILE *src);

void search_files(arraylist_t *conf, arraylist_t *files, char *path);

line_t *get_line(arraylist_t *file, int index);

#endif //ESGILINTER_C_FILELOADER_H


