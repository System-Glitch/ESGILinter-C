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
    int   start_real_line;
    int   start_line_in_buffer;
} line_t;

unsigned int file_row_number(FILE *src);

void file_loader(arraylist_t *e/*, arraylist_t *files*/, arraylist_t *real_file, char *filename);

char* file_to_buffer(FILE *src);

void search_files(arraylist_t *conf, arraylist_t *files, char *path);

line_t *get_line(arraylist_t *file, int index);

void free_buffer(arraylist_t *buffer);

#endif //ESGILINTER_C_FILELOADER_H


