#include <stdlib.h>
#include <stdio.h>
#include <conf.h>
#include "fileloader.h"

int main(int argc, char **argv) {
	//arraylist_t *test;
	//test = file_loader("test.txt");
	arraylist_t *conf = arraylist_init(2);
	load_configuration("test.conf", conf);
	if(conf == NULL) return EXIT_FAILURE;
    printf("Rule : %s\n",((rule_t*)(arraylist_get(conf, 1)))->name); //
    printf("Size : %d\n",conf->size);
	rule_t *test = get_rule(conf, "rule1");
	printf("test %s\n", test->name);
	return EXIT_SUCCESS;
}
