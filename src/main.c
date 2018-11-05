#include <stdlib.h>
#include <stdio.h>
#include <conf.h>
#include "fileloader.h"

int main(int argc, char **argv) {
	//arraylist_t *test;
	//test = file_loader("test.txt");
	load_configuration("test");
	return EXIT_SUCCESS;
}
