#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "conf.h"
#include "display.h"
#include "fileloader.h"

int main(int argc, char **argv) {
	//arraylist_t *test;
	//test = file_loader("test.txt");
	arraylist_t *conf = arraylist_init(2);
	if(conf == NULL) {
		printf("%s[ERROR]%s %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
		return EXIT_FAILURE;
	}

	if(load_configuration("linter.csonf", conf)) {
		printf("%s[ERROR]%s Failed to load config: %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
		arraylist_free(conf, 0);
		return EXIT_FAILURE;
	}

    printf("Rule : %s\n",((rule_t*)(arraylist_get(conf, 1)))->name); //
    printf("Size : %zu\n",conf->size);
	rule_t *test = get_rule(conf, "rule1");
	printf("test %s\n", test->name);
	return EXIT_SUCCESS;
}
