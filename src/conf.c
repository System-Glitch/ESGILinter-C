//
// Created by Pierre Delmer on 23/10/2018.
//

#include "conf.h"

/**
 * Load the configuration array
 * @param char *filename
 * @param arraylist_t *conf
 */
void load_configuration(char *filename, arraylist_t *conf) {
    if (strlen(filename) <= 0) return;

    FILE *src;
    char *line;
    char *tmp;
    rule_t *e;

    src = fopen(filename, "rb");
    if(src == NULL) return;

    line = malloc(sizeof(char) * 1048);
    tmp = malloc(sizeof(char) * 1048);
    e = malloc(sizeof(rule_t));

    exclude_conf_file(conf, filename);

    while(fgets(line, 1048, src) != NULL){
        if(strstr(line, "=extends") ==  line){
            fgets(line, 1048, src);
            strcpy(tmp, "conf_inc_");
            strformat(line, 1048);
            strcat(tmp, line);
            while(line[0] != '\n' && line[0] != '#' && find_rule_index(conf, tmp) == -1 ){
                load_configuration(line, conf);
                fgets(line, 1048, src);
            }
        }else if(strstr(line, "=rules") == line){
            load_rules(src, conf);
        }else if(strstr(line, "=excludedFiles") == line){
            exclude_file(src, conf);
        }else if(strstr(line, "=recursive") == line){
            recursive_activation(src, conf);
        }
    }

    fclose(src);
    free(line);
    free(tmp);
}

/**
 * Load the rules into the conf array
 * @param FILE *src
 * @param arraylist_t *conf
 */

void load_rules(FILE *src, arraylist_t *conf){
    if(src == NULL) return;
    char *line;
    char *name;
    char *tmp;
    int sum;
    int enable;
    int found;
    size_t length;
    rule_t *e;
    const char equal[2] = "=";

    line = malloc(sizeof(char) * 1048);
    name = malloc(sizeof(char) * 1048);

    fgets(line, 1048, src);
    while(!is_whitespace(line[0]) && line[0] != '#'){

        tmp = strtok(line, equal);
        if(tmp == NULL) return;

        if(line[1] == ' '){
            strcpy(name, tmp+2);
        }else{
            strcpy(name, tmp+1);
        }

        if(name[strlen(name) -1 ] == ' '){
            name[strlen(name) -1 ] = '\0';
        }

        tmp = strtok(NULL, equal);
        if(tmp == NULL) return;

        if(tmp[0] == ' ') tmp = tmp+1;
        sum = 0;
        enable = 0;
        length = strlen(tmp);
        for(int i = 0; i  < length ; i++){
            if(tmp[i] >= 48 && tmp[i] <= 57){
                sum += (tmp[i]-48);
                if(i != strlen(tmp) -2){
                    sum *= 10;
                }
            }
            if(sum == 0 && tmp[i] == 'o'){
                if(strstr(tmp, "on")) {
                    enable = 1;
                    break;
                }else{
                    break;
                }
            }
        }
        found = find_rule_index(conf, name);
        if(found >= 0){
            ((rule_t*)(conf->array[found]))->value = sum;
            ((rule_t*)(conf->array[found]))->enable= enable;
        }else if(found == -1){
            e = malloc(sizeof(rule_t));
            e->name = malloc(sizeof(char) * 255);
            strcpy(e->name, name);
            e->value = sum;
            e->enable = enable;
            arraylist_add(conf, e);
        }
        fgets(line, 1048, src);
    }
    free(line);
    free(name);
}

/**
 * Load all the excluded files
 * @param FILE *src
 * @param arraylist_t *conf
 */
void exclude_file(FILE *src, arraylist_t *conf){
    if(src == NULL) return;
    char *line;
    int found;
    char *tmp;
    rule_t *e;

    line = malloc(sizeof(char) * 1048);
    tmp = malloc(sizeof(char) * 1048);

    fgets(line, 1048, src);

    while(!is_whitespace(line[0]) && line[0] != '#'){
        strcpy(tmp, "exfile_");
        if(line[1] == ' '){
            strcat(tmp, line+2);
        }else{
            strcat(tmp, line+1);
        }
        strformat(tmp, 1048);

        found = find_rule_index(conf, tmp);
        if(found == -1 ){
            e = malloc(sizeof(rule_t));
            e->name = malloc(sizeof(char) * 1048);
            strcpy(e->name, tmp);
            e->value = 0;
            e->enable = 0;
            arraylist_add(conf, e);
        }
        fgets(line, 1048, src);
    }
    free(line);
    free(tmp);
}

/**
 * Avoid loop on the extends property
 * @param arraylist_t *conf
 * @param char *name
 */
void exclude_conf_file(arraylist_t *conf, char *name){
    if(strlen(name) <= 0) return;

    rule_t *e;

    e = malloc(sizeof(rule_t));
    e->name = malloc(sizeof(char) * 1048);

    strcpy(e->name, "conf_inc_");
    strcat(e->name, name);

    e->enable = 1;
    e->value = 0;
    arraylist_add(conf, e);
}


/**
 * Load the recursive rule in the configuration file
 * @param FILE *src
 * @param arraylist_t *conf
 */
void recursive_activation(FILE *src, arraylist_t *conf){
    if(src == NULL) return;

    char *line;
    int found;
    int enable;
    enable = 0;
    rule_t *e;

    line = malloc(sizeof(char) * 1048);

    fgets(line, 1048, src);
    if(!is_whitespace(line[0]) && line[0] != '#'){

        found = find_rule_index(conf, "recursive");

        if(strstr(line, "true")){
            enable = 1;
        }
        if(strstr(line, "false")){
            enable = 0;
        }

        if(found >= 0){
            ((rule_t*)(conf->array[found]))->enable= enable;
        }else if(found == -1){
            e = malloc(sizeof(rule_t));

            e->name = malloc(sizeof(char) * 1048);

            strcpy(e->name, "recursive");
            e->value = 0;
            e->enable = enable;
            arraylist_add(conf, e);
        }
    }
    free(line);
}

/**
 *  Verifiy if a rule is rewritable
 * @param arraylist_t *conf
 * @param char *name
 * @return int
 */

int find_rule_index(arraylist_t *conf, char *name){
    if(strlen(name) <= 0) return -2;

    for(int i = 0; i < conf->size; i++){
        if(strcmp(name, ((rule_t*)(conf->array[i]))->name) == 0){
            return i;
        }
    }
    return -1;
}
/**
 * Verify if the linter is recursive
 * @param arraylist_t *conf
 * @return int
 */
int is_recursive(arraylist_t *conf){
    if(conf == NULL) return NULL;
    int index = find_rule_index(conf, "recursive");
    if(index >= 0) return ((rule_t*)(arraylist_get(conf, index)))->enable;
    return -1;
}

/**
 *  Return a rule with its params
 * @param arraylist_t *conf
 * @param char *name
 * @return rule_t*
 */
rule_t *get_rule(arraylist_t *conf, char *name){
    if(conf == NULL || strlen(name) <= 0) return NULL;
    int index = find_rule_index(conf, name);
    if(index >= 0) return ((rule_t*)(arraylist_get(conf, index)));
    return NULL;
}