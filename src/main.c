#include <stdlib.h>
#include <stdio.h>
#include <conf.h>
#include "fileloader.h"

int main(int argc, char **argv) {
    char *path;

	arraylist_t *conf = arraylist_init(2);
	arraylist_t *files = arraylist_init(2);
	arraylist_t *buffer = arraylist_init(2);
	arraylist_t *real_file = arraylist_init(2);

    path = malloc(sizeof(char) * 255);

    strcpy(path, "."); // replace with an argument

	load_configuration("test.conf", conf); // replace with an argument

	if(conf == NULL) return EXIT_FAILURE;

	search_files(conf, files, path);

	for(int i = 0; i < files->size; i++){

		file_loader(buffer, files, real_file, "test.txt"); // replace "test.txt" by files->array[i]
		printf("\n");
		for(int j = 0; j < buffer->size; j++){
			printf("New line : %s\n",get_line(buffer,j)->line);
		}
		break;
		/*
		 *
		 * TREAT DATA FROM HERE
		 *
		 * APPLY RULES
		 *
		 */
	}
	return EXIT_SUCCESS;
}
