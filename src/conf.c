//
// Created by Pierre Delmer on 23/10/2018.
//

#include "conf.h"

/*
 * Load the configuration array
 * @params char*
 * @return arraylist_t
 */
void load_configuration(char *filename) {
    if (strlen(filename) <= 0) return;
    arraylist_t *conf;
    conf = file_loader("test.txt", 1);
    printf("\n\nTest : %s\n", conf->array[2]);
}