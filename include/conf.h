//
// Created by Pierre Delmer on 23/10/2018.
//

#ifndef ESGILINTER_C_CONF_H
#define ESGILINTER_C_CONF_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "arraylist.h"
#include "fileloader.h"

typedef struct rule {
    char *name;
    int   enable;
    int   value;
} rule_t;

void load_configuration(char *conf);

#endif //ESGILINTER_C_CONF_H
