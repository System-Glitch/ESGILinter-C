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
	arraylist_t *buffer = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_t *real_file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);

	if(conf == NULL || files == NULL || buffer == NULL || real_file == NULL) {
		printf("%s[ERROR]%s %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
		return EXIT_FAILURE;
	}

	path = malloc(sizeof(char) * 255);
	strcpy(path, "."); // replace with an argument

	if(load_configuration("test.conf", conf)) { //replace with argument
		printf("%s[ERROR]%s Failed to load config: %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
		arraylist_free(conf, 0);
		return EXIT_FAILURE;
	}

	search_files(conf, files, path);

	for(int i = 0; i < files->size; i++){
		file_loader(buffer, files, real_file, "test.txt"); // replace "test.txt" by files->array[i]
		printf("\n");
		for(int j = 0; j < buffer->size; j++){
			printf("New line : %s\n",get_line(buffer,j)->line);
		}





		arraylist_free(real_file, 1);
		free_buffer(buffer);

		break;
		/*
		 *
		 * TREAT DATA FROM HERE
		 *
		 * APPLY RULES
		 *
		 */
	}

	arraylist_free(files, 1);

	free_conf(conf);

	//test();
	return EXIT_SUCCESS;
}
