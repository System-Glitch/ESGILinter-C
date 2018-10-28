#include "test.h"
#include "display.h"
#include "arraylist.h"

void test() {
	arraylist_t *list = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	int *value;


	for(unsigned int i = 0 ; i < 25 ; i++) {
		value = malloc(sizeof(int));
		*value = i;
		arraylist_add(list, value);
	}

	printf("--------------------%s\n", FORMAT_RESET);
	printf("%sTESTING ARRAY LIST%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);
	for(size_t i = 0 ; i < list->size; i++)
		printf("%s- %s%d\n", COLOR_GREY, FORMAT_RESET, *((int*)arraylist_get(list, i)));

	printf("%sFreeing array list%s\n", COLOR_CYAN, FORMAT_RESET);
	arraylist_free(list);
	printf("--------------------%s\n", FORMAT_RESET);



	//-------- TEST displayLinterError ---------
	char* testLine = "for(unsigned int i = 0; i < (errorIndex -1); i++){";
	char* testFileName = "ghostFile.c";
	int testErrorIndex = 42;
	int testLineNb = 3;

	displayLinterError(testFileName, testLineNb, testLine, testErrorIndex);
	//------------------------------------------

}
