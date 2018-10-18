#include "test.h"
#include "display.h"
#include "arraylist.h"
#include "scopetree.h"

static void test_variable_declaration_parsing(char *line) {
	arraylist_t *list = NULL;
	field_t *variable = NULL;
	
	list = get_variables_from_declaration(line);

	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, line, FORMAT_RESET);
	if(list != NULL) {
		printf("%sOutput: %s\n", COLOR_BLUE, FORMAT_RESET);
		for(unsigned int i = 0 ; i < list->size ; i++) {
			variable = arraylist_get(list, i);
			printf(" %s%2d:  %sName:       %s%s\n      %sType:       %s%s\n      %sIs pointer: %s%d\n", 
				COLOR_MAGENTA, i,
				COLOR_CYAN, FORMAT_RESET, variable->name,
				COLOR_CYAN, FORMAT_RESET, variable->type.name,
				COLOR_CYAN, FORMAT_RESET, variable->type.is_pointer);
			field_free(variable);
		}
		arraylist_free(list, 0);
	} else
		printf("%sOutput: %sNULL%s\n", COLOR_BLUE, COLOR_RED, FORMAT_RESET);
}

void test() {
	arraylist_t *list = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	int *value;
	
	for(unsigned int i = 0 ; i < 25 ; i++) {
		value = malloc(sizeof(int));
		*value = i;
		arraylist_add(list, value);
	}

	printf("------------------------------%s\n", FORMAT_RESET);
	printf("%sTESTING ARRAY LIST%s\n%s-> %s", COLOR_GREEN_BOLD, FORMAT_RESET, COLOR_GREY, FORMAT_RESET);
	for(size_t i = 0 ; i < list->size; i++)
		printf("%d ", *((int*)arraylist_get(list, i)));

	printf("\n%sFreeing array list%s\n", COLOR_CYAN, FORMAT_RESET);
	arraylist_free(list, 1);
	printf("------------------------------%s\n", FORMAT_RESET);
	printf("%sTESTING PARSE VARIABLE DECLARATION%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);
	
	test_variable_declaration_parsing("int i;");
	test_variable_declaration_parsing("char* *ets = \"sdf_\";");
	test_variable_declaration_parsing("char \nk\n\t =             'a';");
	test_variable_declaration_parsing("char array[12] = \"abcdefg\";");
	test_variable_declaration_parsing("char array[12][4];");
	test_variable_declaration_parsing("char* *array[12] = \"abcdefg\";");
	test_variable_declaration_parsing("unsigned char c = 42;");
	test_variable_declaration_parsing("unsigned char* c = 42;");
	test_variable_declaration_parsing("unsigned char c[42];");
	test_variable_declaration_parsing("unsigned* char c[42];");
	test_variable_declaration_parsing("char * spacing;");
	test_variable_declaration_parsing("unsigned long * spacing;");
	test_variable_declaration_parsing("unsigned long ** spacing;");
	test_variable_declaration_parsing("unsigned long * * spacing;");
	test_variable_declaration_parsing("int i,k;");
	test_variable_declaration_parsing("int i = 0,k, l = 25;");
	test_variable_declaration_parsing("int *i, *k, **l, m;");
	test_variable_declaration_parsing("int* one, *two;;");

	printf("------------------------------%s\n", FORMAT_RESET);
}
