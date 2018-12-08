//
// Created by Pierre Delmer on 23/10/2018.
//

#ifndef ESGILINTER_C_CONF_H
#define ESGILINTER_C_CONF_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "arraylist.h"
#include "fileloader.h"
#include "stringutils.h"

typedef struct rule {
    char *name;
    int   enable;
    int   value;
} rule_t;

char load_configuration(char *filename, arraylist_t *conf);

void load_rules(FILE *src, arraylist_t *conf);

void exclude_file(FILE *src, arraylist_t *conf);

void recursive_activation(FILE *src, arraylist_t *conf);

int find_rule_index(arraylist_t *conf, char *name);

int is_recursive(arraylist_t *conf);

rule_t *get_rule(arraylist_t *conf, char *name);

void exclude_conf_file(arraylist_t *conf, char *name);

int is_excluded(arraylist_t *conf, char *path);

void exclude_filepath(arraylist_t *conf, char *filepath);

void free_conf(arraylist_t *conf);

unsigned char check_rule(arraylist_t *conf, char *rule_name);

#endif //ESGILINTER_C_CONF_H
