//
// Created by Pierre Delmer on 23/10/2018.
//

#ifndef ESGILINTER_C_FILELOADER_H
#define ESGILINTER_C_FILELOADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "arraylist.h"

unsigned int file_row_number(FILE *src);

arraylist_t* file_loader(char *filename);

unsigned long file_length(FILE *src);

char* file_to_buffer(FILE *src);

#endif //ESGILINTER_C_FILELOADER_H


