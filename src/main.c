#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "conf.h"
#include "display.h"
#include "fileloader.h"
#include "test.h"

int main(int argc, char **argv) {
	char *path;
	arraylist_t *conf = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_t *files = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_t *buffer = NULL;
	arraylist_t *real_file = NULL;

	if(conf == NULL || files == NULL) {
		printf("%s[ERROR]%s %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
		return EXIT_FAILURE;
	}

	path = malloc(sizeof(char) * 255);
	strcpy(path, "."); // replace with an argument

	if(load_configuration("linter.conf", conf)) { //replace with argument
		printf("%s[ERROR]%s Failed to load config: %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
		arraylist_free(conf, 0);
		return EXIT_FAILURE;
	}

	search_files(conf, files, path);

	for(int i = 0; i < files->size; i++){
		buffer = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
		real_file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);

		if(buffer == NULL || real_file == NULL) {
			printf("%s[ERROR]%s %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
			return EXIT_FAILURE;
		}

		file_loader(buffer, files, real_file, arraylist_get(files, i));

		/*
		 *
		 * TREAT DATA FROM HERE
		 *
		 * APPLY RULES
		 *
		 */
		arraylist_free(real_file, 1);
		free_buffer(buffer);
		break;
	}
	arraylist_free(files, 1);

	free_conf(conf);
    free(path);
	return EXIT_SUCCESS;
}
