#include "test.h"
#include "display.h"
#include "arraylist.h"
#include "linkedlist.h"
#include "scopetree.h"
#include "parsing_type.h"
#include "parsing_variables.h"
#include "parsing_functions.h"
#include "rules/no_prototype.h"
#include "rules/max_line_numbers.h"

static void print_variables(arraylist_t *variables, unsigned int tabs) {
	field_t *variable = NULL;
	if(variables != NULL) {
		for(unsigned int i = 0 ; i < variables->size ; i++) {
			variable = arraylist_get(variables, i);

			for(unsigned int j = 0 ; j < tabs ; j++)
				printf("\t");

			printf(" %s%2d:  %sName:       %s%s\n",
				COLOR_MAGENTA, i,
				COLOR_CYAN, FORMAT_RESET, variable->name);

			for(unsigned int j = 0 ; j < tabs ; j++)
				printf("\t");
			printf("      %sType:       %s%s\n", COLOR_CYAN, FORMAT_RESET, variable->type.name);

			for(unsigned int j = 0 ; j < tabs ; j++)
				printf("\t");
			printf("      %sIs pointer: %s%d\n", COLOR_CYAN, FORMAT_RESET, variable->type.is_pointer);
		}
	}
}

static void print_function(function_t *function, unsigned int tabs) {
	if(function != NULL) {
		for(unsigned int j = 0 ; j < tabs ; j++)
			printf("\t");
		printf("      %sName:         %s%s\n", COLOR_CYAN, FORMAT_RESET, function->name);

		for(unsigned int j = 0 ; j < tabs ; j++)
			printf("\t");
		printf("      %sIs prototype: %s%d\n", COLOR_CYAN, FORMAT_RESET, function->is_prototype);

		for(unsigned int j = 0 ; j < tabs ; j++)
			printf("\t");
		printf("      %sType:         %s%s\n", COLOR_CYAN, FORMAT_RESET, function->return_type.name);

		for(unsigned int j = 0 ; j < tabs ; j++)
			printf("\t");
		printf("      %sIs pointer:   %s%d\n", COLOR_CYAN, FORMAT_RESET, function->return_type.is_pointer);

		for(unsigned int j = 0 ; j < tabs ; j++)
			printf("\t");
		printf("      %sParameters:   %s", COLOR_CYAN, FORMAT_RESET);
		if(function->params->size == 0) {
			printf("%sNone%s\n", COLOR_RED, FORMAT_RESET);
		} else {
			printf("\n");
			print_variables(function->params, tabs + 1);
		}
	}
}

static void print_functions(arraylist_t *functions, unsigned int tabs) {
	function_t *function = NULL;
	if(functions != NULL) {
		for(unsigned int i = 0 ; i < functions->size ; i++) {
			function = arraylist_get(functions, i);
			print_function(function, tabs);
		}
	}
}

static void test_function_declaration_parsing(char *line) {
	function_t *function = get_function_from_declaration(0, line);

	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, line, FORMAT_RESET);
	if(function != NULL) {
		printf("%sOutput: %s\n", COLOR_BLUE, FORMAT_RESET);
		print_function(function, 0);
		function_free(function);
	} else
		printf("%sOutput: %sNULL%s\n", COLOR_BLUE, COLOR_RED, FORMAT_RESET);
}

static void test_variable_declaration_parsing(char *line) {
	arraylist_t *list = NULL;

	list = get_variables_from_declaration(0, line);

	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, line, FORMAT_RESET);
	if(list != NULL) {
		printf("%sOutput: %s\n", COLOR_BLUE, FORMAT_RESET);
		print_variables(list, 0);
		field_list_free(list);
	} else
		printf("%sOutput: %sNULL%s\n", COLOR_BLUE, COLOR_RED, FORMAT_RESET);
}

static void print_scope(scope_t *scope, unsigned int level) {

	node_t *current;

	if(scope == NULL) return;

	for(unsigned int i = 0 ; i < level ; i++)
		printf("\t");
	printf("From %d to %d\n", scope->from_line, scope->to_line);

	if(scope->parent == NULL) { //Is root scope
		for(unsigned int i = 0 ; i < level ; i++)
			printf("\t");
		printf("%sFunctions:%s ", COLOR_CYAN_BOLD, FORMAT_RESET);
		if(scope->functions->size == 0) printf("%sNone%s", COLOR_RED, FORMAT_RESET);
		printf("\n");
		print_functions(scope->functions, level);
	}

	for(unsigned int i = 0 ; i < level ; i++)
		printf("\t");
	printf("%sVariables:%s ", COLOR_CYAN_BOLD, FORMAT_RESET);
	if(scope->variables->size == 0) printf("%sNone%s", COLOR_RED, FORMAT_RESET);
	printf("\n");
	print_variables(scope->variables, level);

	for(unsigned int i = 0 ; i < level ; i++)
		printf("\t");
	printf("%sChildren:%s  ", COLOR_CYAN_BOLD, FORMAT_RESET);
	current = scope->children->head;
	if(current != NULL) {
		printf("\n");
		do {
			print_scope(current->val, level + 1);
		} while ((current = current->next) != NULL);
	} else {
		printf("%sNone%s\n", COLOR_RED, FORMAT_RESET);
	}

	for(unsigned int i = 0 ; i < level ; i++)
		printf("\t");
	printf("%s--------------------%s\n", FORMAT_DIM, FORMAT_RESET);
}

static void test_variable_parsing() {
	printf("------------------------------%s\n", FORMAT_RESET);
	printf("%sTESTING PARSE VARIABLE DECLARATION%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	test_variable_declaration_parsing("int i;");
	test_variable_declaration_parsing("\tint i = 5;");
	test_variable_declaration_parsing("int t i = 5;");
	test_variable_declaration_parsing("char* *ets = \"sdf_\";");
	test_variable_declaration_parsing("char \nk\n\t =             'a';");
	test_variable_declaration_parsing("char array[12] = \"abcdefg\";");
	test_variable_declaration_parsing("char array[12][4];");
	test_variable_declaration_parsing("char* *array[12] = \"abcdefg\";");
	test_variable_declaration_parsing("unsigned char c = 42;");
	test_variable_declaration_parsing("unsigned char* c = 42;");
	test_variable_declaration_parsing("unsigned char c[42];");
	test_variable_declaration_parsing("unsigned* char c[42];");
	test_variable_declaration_parsing("unsigned** char c[42];");
	test_variable_declaration_parsing("unsigned* char** c[42];");

	test_variable_declaration_parsing("ch[ar var;");
	test_variable_declaration_parsing("un*si()gned ch[ar var;");
	test_variable_declaration_parsing("size_t length = 5;");

	test_variable_declaration_parsing("char * spacing;");
	test_variable_declaration_parsing("unsigned long * spacing;");
	test_variable_declaration_parsing("unsigned long ** spacing;");
	test_variable_declaration_parsing("unsigned long * * spacing;");

	test_variable_declaration_parsing("int i,k;");
	test_variable_declaration_parsing("int i = 0,k, l = 25;");
	test_variable_declaration_parsing("int *i, *k, **l, m;");
	test_variable_declaration_parsing("int* one, *two;;");

	test_variable_declaration_parsing("int i; int j;");
	test_variable_declaration_parsing("unsigned long long int* one, *two;");

	test_variable_declaration_parsing("int;");
	test_variable_declaration_parsing("unsigned int;");
	test_variable_declaration_parsing("unsigned int ;");
	test_variable_declaration_parsing("qwerty;");
	test_variable_declaration_parsing("qwerty = 5;");
	test_variable_declaration_parsing("qwerty int;");

	test_variable_declaration_parsing("\t//int j = 88;");
	test_variable_declaration_parsing("\tprintf(\"%d\", i);");
	test_variable_declaration_parsing("\t/*int j = 88;*/");
	test_variable_declaration_parsing("/* comment */ double db = 1.2;");
	test_variable_declaration_parsing("/* comment */ for(int count = 0 ; count < 5 ; count++) {}");

	test_variable_declaration_parsing("extern unsigned char c = 42;");

}

static void test_scope_parsing() {
	printf("------------------------------%s\n", FORMAT_RESET);

	printf("%sTESTING PARSE SCOPE%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	arraylist_t *file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("static int glob = 89;"));

	arraylist_add(file, strduplicate("void test(int param) {"));
	arraylist_add(file, strduplicate("\tchar c = 'c';"));
	arraylist_add(file, strduplicate("\tprintf(\"%c %d\", c, i);"));
	arraylist_add(file, strduplicate("}"));

	arraylist_add(file, strduplicate("int main() {"));
	arraylist_add(file, strduplicate("\tint i = 42;"));
	arraylist_add(file, strduplicate("\tchar array[] = {'a','b','c','d'};"));
	arraylist_add(file, strduplicate("\tprintf(\"%d\", i);"));
	arraylist_add(file, strduplicate("\tif(i == 42) {"));
	arraylist_add(file, strduplicate("\t\tchar c = array[1];"));
	arraylist_add(file, strduplicate("\t}"));
	arraylist_add(file, strduplicate("}"));

	scope_t *scope = parse_root_scope(file);
	if(scope != NULL) {
		print_scope(scope, 0);
		scope_free(scope);
	}

	arraylist_free(file, 1);

	file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("int main() {"));
	arraylist_add(file, strduplicate("\tint i = 42;"));
	arraylist_add(file, strduplicate("\tif(i == 42) {"));
	arraylist_add(file, strduplicate("\t\tchar c = array[1];"));
	//Missing closing parenthesis
	arraylist_add(file, strduplicate("}"));

	scope = parse_root_scope(file);
	if(scope != NULL) {
		print_scope(scope, 0);
		scope_free(scope);
	} else {
		printf("%sScope %sNULL%s, possible syntax error.\n%s", COLOR_YELLOW, COLOR_RED, COLOR_YELLOW, FORMAT_RESET);
	}

	arraylist_free(file, 1);

	file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("int main() {"));
	arraylist_add(file, strduplicate("/* comment */ double db = 1.2;"));
	arraylist_add(file, strduplicate("\tfor(int i = 0, j = 5 ; i < 10 ; i++) {"));
	arraylist_add(file, strduplicate("\t\tprintf(\"%d\", i);"));
	arraylist_add(file, strduplicate("\t}"));
	arraylist_add(file, strduplicate("}"));

	scope = parse_root_scope(file);
	if(scope != NULL) {
		print_scope(scope, 0);
		scope_free(scope);
	} else {
		printf("%sScope %sNULL%s, possible syntax error.\n%s", COLOR_YELLOW, COLOR_RED, COLOR_YELLOW, FORMAT_RESET);
	}

	arraylist_free(file, 1);
}

static void test_function_parsing() {
	printf("------------------------------%s\n", FORMAT_RESET);

	printf("%sTESTING PARSE FUNCTIONS%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	test_function_declaration_parsing("void test();");
	test_function_declaration_parsing("void test2(int, int);");
	test_function_declaration_parsing("void test3(int i, int* j);");
	test_function_declaration_parsing("void test4(int i , unsigned int *ptr) {");
	test_function_declaration_parsing("void *test_ptr() {");
	test_function_declaration_parsing("void* *test_ptr2() {");
	test_function_declaration_parsing("void * test_ptr3() {");
	test_function_declaration_parsing("void[15] array() {");
	test_function_declaration_parsing("void test2(int) {");
	test_function_declaration_parsing("void test2(int array[15]) {");
	test_function_declaration_parsing("void test2(int* array[15]) {");
}

static void test_rule_max_line_numbers(){
	printf("------------------------------%s\n", FORMAT_RESET);

	printf("%sTESTING RULE: max-line-numbers%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	arraylist_t *file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("static int glob = 89;"));

	max_line_numbers(file, 5);
	max_line_numbers(file, 50);

}

static void test_rule_no_prototype() {

	printf("------------------------------%s\n", FORMAT_RESET);

	printf("%sTESTING RULE: no-prototype%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	arraylist_t *file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("static int glob = 89;"));
	arraylist_add(file, strduplicate("void test(int param);"));

	arraylist_add(file, strduplicate("char test(int param) {"));
	arraylist_add(file, strduplicate("\tchar c = 'c';"));
	arraylist_add(file, strduplicate("\tprintf(\"%c %d\", c, i);"));
	arraylist_add(file, strduplicate("}"));

	arraylist_add(file, strduplicate("int main() {"));
	arraylist_add(file, strduplicate("\tint i = 42;"));
	arraylist_add(file, strduplicate("}"));

	arraylist_add(file, strduplicate("char* test2(char v) {"));
	arraylist_add(file, strduplicate("\tint i = 42;"));
	arraylist_add(file, strduplicate("}"));

	scope_t *scope = parse_root_scope(file);
	if(scope != NULL) {
		printf("Return: %d\n", check_no_prototype(scope, file));
		scope_free(scope);
	} else {
		printf("%sScope %sNULL\n%s", COLOR_YELLOW, COLOR_RED, FORMAT_RESET);
	}
	arraylist_free(file, 1);
}

static void test_function_call_parsing() {
	function_t *function = parse_function_call(0 , "printf(\"%d\", variable);");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}

	function = parse_function_call(0 , "test  (int);");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}

	function = parse_function_call(0 , "test(a, b)");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}

	function = parse_function_call(0 , "test()");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}

	function = parse_function_call(0 , "test(')')");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}

	function = parse_function_call(0 , "test(\"bla()\")");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}

	function = parse_function_call(0 , "test(bla())");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}

	function = parse_function_call(0 , "test(bla))");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}
}

static void test_expression_type(char *line, scope_t *scope) {
	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, line, FORMAT_RESET);

	char *undefined_variable = NULL;
	char *undefined_function = NULL;
	type_t type = get_expression_type(line, scope, &undefined_variable, &undefined_function);
	printf("%sOutput: %s\n", COLOR_BLUE, FORMAT_RESET);
	printf("\t%sType:       %s%s\n", COLOR_CYAN, FORMAT_RESET, type.name);
	printf("\t%sIs pointer: %s%d\n", COLOR_CYAN, FORMAT_RESET, type.is_pointer);

	if(strcmp(type.name, "NULL"))
		free(type.name);
	if(undefined_variable != NULL) {
		printf("\t%sUndefined variable: %s%s\n", COLOR_RED, FORMAT_RESET, undefined_variable);
		free(undefined_variable);
	}
	if(undefined_function != NULL) {
		printf("\t%sUndefined function: %s%s\n", COLOR_RED, FORMAT_RESET, undefined_function);
		free(undefined_function);
	}
}

static void test_parse_expression_type() {

	printf("------------------------------%s\n", FORMAT_RESET);
	printf("%sTESTING PARSE EXPRESSION TYPE%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	arraylist_t *file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("static int glob = 89;"));
	arraylist_add(file, strduplicate("unsigned int variable = 89;"));
	arraylist_add(file, strduplicate("void function(int param);"));

	arraylist_add(file, strduplicate("char function(int param) {"));
	arraylist_add(file, strduplicate("\tchar c = 'c';"));
	arraylist_add(file, strduplicate("\tprintf(\"%c %d\", c, i);"));
	arraylist_add(file, strduplicate("}"));

	arraylist_add(file, strduplicate("int main() {"));
	arraylist_add(file, strduplicate("\tint i = 42;"));
	arraylist_add(file, strduplicate("}"));

	arraylist_add(file, strduplicate("char* test2(char v) {"));
	arraylist_add(file, strduplicate("\tint i = 42;"));
	arraylist_add(file, strduplicate("}"));

	scope_t *scope = parse_root_scope(file);

	test_expression_type("(char)c", scope);
	test_expression_type("(  unsigned char   ) c", scope);
	test_expression_type("'d'", scope);
	test_expression_type("\"d\"", scope);
	test_expression_type("function()", scope);
	test_expression_type("test()", scope);
	test_expression_type("variable", scope);
	test_expression_type("*& &  variable", scope);

	scope_free(scope);
}

void test() {

	test_variable_parsing();
	test_function_parsing();
	test_scope_parsing();
	test_rule_max_line_numbers();
	test_rule_no_prototype();
	test_function_call_parsing();
	test_parse_expression_type();

	printf("------------------------------%s\n", FORMAT_RESET);
}
