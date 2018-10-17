#include "test.h"
#include "display.h"
#include "arraylist.h"
#include "scopetree.h"

void test() {
	arraylist_t *list = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	int *value;
	field_t *variable = NULL;

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
	
	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, "int i;", FORMAT_RESET);
	variable = get_variable_from_declaration("int i;");
	printf("%sOutput: %s\n\t%sName:       %s%s\n\t%sType:       %s%s\n\t%sIs pointer: %s%d\n", 
		COLOR_BLUE, FORMAT_RESET,
		COLOR_CYAN, FORMAT_RESET, variable->name,
		COLOR_CYAN, FORMAT_RESET, variable->type.name,
		COLOR_CYAN, FORMAT_RESET, variable->type.is_pointer);
	field_free(variable);

	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, "char* *ets = \"sdf_\";", FORMAT_RESET);
	variable = get_variable_from_declaration("char* *ets = \"sdf_\";");
	printf("%sOutput: %s\n\t%sName:       %s%s\n\t%sType:       %s%s\n\t%sIs pointer: %s%d\n", 
		COLOR_BLUE, FORMAT_RESET,
		COLOR_CYAN, FORMAT_RESET, variable->name,
		COLOR_CYAN, FORMAT_RESET, variable->type.name,
		COLOR_CYAN, FORMAT_RESET, variable->type.is_pointer);
	field_free(variable);

	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, "char \nk\n\t =             'a';", FORMAT_RESET);
	variable = get_variable_from_declaration("char \nk\n\t =             'a';");
	printf("%sOutput: %s\n\t%sName:       %s%s\n\t%sType:       %s%s\n\t%sIs pointer: %s%d\n", 
		COLOR_BLUE, FORMAT_RESET,
		COLOR_CYAN, FORMAT_RESET, variable->name,
		COLOR_CYAN, FORMAT_RESET, variable->type.name,
		COLOR_CYAN, FORMAT_RESET, variable->type.is_pointer);
	field_free(variable);

	printf("------------------------------%s\n", FORMAT_RESET);
}
