#include "test.h"
#include "display.h"
#include "arraylist.h"
#include "scopetree.h"

static void test_variable_declaration_parsing(char *line) {
	field_t *variable = NULL;
	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, line, FORMAT_RESET);
	variable = get_variable_from_declaration(line);
	if(variable != NULL) {
		printf("%sOutput: %s\n\t%sName:       %s%s\n\t%sType:       %s%s\n\t%sIs pointer: %s%d\n", 
			COLOR_BLUE, FORMAT_RESET,
			COLOR_CYAN, FORMAT_RESET, variable->name,
			COLOR_CYAN, FORMAT_RESET, variable->type.name,
			COLOR_CYAN, FORMAT_RESET, variable->type.is_pointer);
		field_free(variable);
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
	arraylist_free(list);
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

	printf("------------------------------%s\n", FORMAT_RESET);
}
