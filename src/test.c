#include "test.h"
#include "display.h"
#include "arraylist.h"
#include "linkedlist.h"
#include "scopetree.h"
#include "parsing_type.h"
#include "parsing_variables.h"
#include "parsing_expressions.h"
#include "parsing_functions.h"
#include "parsing_operations.h"
#include "rules/no_prototype.h"
#include "rules/parsing.h"
#include "rules/no_multi_declaration.h"

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
	test_variable_declaration_parsing("static unsigned char b = 98;");
	test_variable_declaration_parsing("const unsigned variable = 98;");
	test_variable_declaration_parsing("\tchar *ret = test2(param, param3, 'c');");
	test_variable_declaration_parsing("\tchar *ret = test2(param, param3, 'c'), ret2, ret3 = NULL;");
	test_variable_declaration_parsing("signed int sign;");
	test_variable_declaration_parsing("signed int volatile;");

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
	arraylist_add(file, strduplicate("\t\tconst char *literal = \"test { int k = 0; }\";"));
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

	file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("int main() {"));
	arraylist_add(file, strduplicate("\tswitch (1) {"));
	arraylist_add(file, strduplicate("\tcase 1: test();"));
	arraylist_add(file, strduplicate("\tint a;"));
	arraylist_add(file, strduplicate("\tbreak;"));
	arraylist_add(file, strduplicate("\tcase 2: int b;")); //5
	arraylist_add(file, strduplicate("\tcase 3: if(b) {"));
	arraylist_add(file, strduplicate("\tint inner;"));
	arraylist_add(file, strduplicate("\t}"));
	arraylist_add(file, strduplicate("\tswitch(b) {"));
	arraylist_add(file, strduplicate("\tint inner2;")); //10
	arraylist_add(file, strduplicate("\t}"));
	arraylist_add(file, strduplicate("\tcase 4: switch(b) {"));
	arraylist_add(file, strduplicate("\tint inner3;"));
	arraylist_add(file, strduplicate("\t}"));
	arraylist_add(file, strduplicate("\tcase 3: case 4: int c;")); //15
	arraylist_add(file, strduplicate("\tcase 5:"));
	arraylist_add(file, strduplicate("\t\tif(1) {"));
	arraylist_add(file, strduplicate("\t\tint fi;"));
	arraylist_add(file, strduplicate("\t\t}"));
	arraylist_add(file, strduplicate("\tdefault: int d;")); //20
	arraylist_add(file, strduplicate("\tint e;"));
	arraylist_add(file, strduplicate("\t}"));
	arraylist_add(file, strduplicate("\tswitch(test) { }"));
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

	printf("------------------------------%s\n", FORMAT_RESET);
	printf("%sTESTING FUNCTION CALL PARSING%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

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

	function = parse_function_call(0 , "test(bla) + b");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}

	function = parse_function_call(0 , "test(\"bla,bla\")");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}

	function = parse_function_call(0 , "test(\"bla,bla,\" , bla)");
	if(function == NULL) {
		printf("%sSyntax error%s\n", COLOR_RED, FORMAT_RESET);
	} else {
		print_function(function, 0);
		function_free(function);
	}
}

static void test_expression(char *line, unsigned int line_index, scope_t *scope) {
	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, line, FORMAT_RESET);

	messages_t *messages = malloc(sizeof(messages_t));
	messages->undeclared_functions = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	messages->undeclared_variables = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	messages->invalid_params       = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	messages->invalid_calls        = NULL;
	messages->variables_list       = NULL;
	messages->functions_list       = NULL;
	messages->wrong_assignment     = NULL;
	type_t type = parse_expression(line, line_index, scope, messages);
	printf("%sOutput: %s\n", COLOR_BLUE, FORMAT_RESET);
	printf("\t%sType:       %s%s\n", COLOR_CYAN, FORMAT_RESET, type.name);
	printf("\t%sIs pointer: %s%d\n", COLOR_CYAN, FORMAT_RESET, type.is_pointer);
	
	free(type.name);

	for(size_t i = 0 ; i < messages->undeclared_functions->size ; i++) {
		printf("\t%sUndefined function: %s%s\n", COLOR_RED, FORMAT_RESET, ((function_t*)arraylist_get(messages->undeclared_functions, i))->name);
	}

	for(size_t i = 0 ; i < messages->undeclared_variables->size ; i++) {
		printf("\t%sUndefined variable: %s%s\n", COLOR_RED, FORMAT_RESET, (char*)arraylist_get(messages->undeclared_variables, i));
	}

	for(size_t i = 0 ; i < messages->invalid_params->size ; i++) {
		printf("\t%sInvalid param: %s%s\n", COLOR_RED, FORMAT_RESET, ((field_t*)arraylist_get(messages->invalid_params, i))->name);
	}

	function_list_free(messages->undeclared_functions);
	field_list_free(messages->invalid_params);
	arraylist_free(messages->undeclared_variables, 1);
	free(messages);
}

static void test_parse_expression() {

	printf("------------------------------%s\n", FORMAT_RESET);
	printf("%sTESTING PARSE EXPRESSION%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	arraylist_t *file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("static int glob = 89;"));
	arraylist_add(file, strduplicate("unsigned int variable = 89;"));
	arraylist_add(file, strduplicate("static unsigned char b = 98;"));
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

	test_expression("(char)c", 0, scope);
	test_expression("(char) ", 0, scope);
	test_expression("(char) c", 0, scope);
	test_expression("(char) test('c')", 0, scope);
	test_expression("b", 5, scope);
	test_expression("(  unsigned char   ) c", 0, scope);
	test_expression("'d'", 0, scope);
	test_expression("\"d\"", 0, scope);
	test_expression("function()", 10, scope);
	test_expression("test2()", 20, scope);
	test_expression("/*comment */test2()", 20, scope);
	test_expression("/*comment */ test2(/* comment */)", 20, scope);
	test_expression("variable", 4, scope);
	test_expression("*& &  variable", 4, scope);
	test_expression("--  variable", 4, scope);
	test_expression("++  variable", 4, scope);
	test_expression("variable++", 4, scope);
	test_expression("variable--", 4, scope);
	test_expression("++variable--", 4, scope);
	test_expression("4", 0, scope);
	test_expression("123456789", 0, scope);
	test_expression("4u", 0, scope);
	test_expression("4l", 0, scope);
	test_expression("4lu", 0, scope);
	test_expression("1.2", 0, scope);
	test_expression("1.2f", 0, scope);
	test_expression(".4", 0, scope);
	test_expression(".4f", 0, scope);
	test_expression(".4l", 0, scope);
	test_expression(".4 l", 0, scope);
	test_expression(". 4", 0, scope);
	test_expression("\"string \\\" quote\"", 0, scope);
	test_expression("\"test * something\"", 0, scope);
	test_expression("\"/* comment */\"", 0, scope);
	test_expression("!function('c')", 20, scope);
	test_expression("!!!function(4)", 20, scope);

	test_expression("!b", 9, scope);
	test_expression("!!b", 9, scope);
	test_expression("!++b", 9, scope);
	test_expression("!*&b", 9, scope);
	test_expression("!&b", 9, scope);

	scope_free(scope);
	arraylist_free(file, 1);
}

static void test_rule_parsing() {

	printf("------------------------------%s\n", FORMAT_RESET);
	printf("%sTESTING RULES WITH PARSING%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	arraylist_t *file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("static int glob = 89;"));

	arraylist_add(file, strduplicate("void function(int param);"));
	arraylist_add(file, strduplicate("void proto();"));

	arraylist_add(file, strduplicate("void declared() {"));
	arraylist_add(file, strduplicate("\ttest2('c');"));
	arraylist_add(file, strduplicate("}"));

	arraylist_add(file, strduplicate("char* test2(char v);"));


	arraylist_add(file, strduplicate("char function(int param, char param2) {"));
	arraylist_add(file, strduplicate("\tchar c = 'c';"));
	arraylist_add(file, strduplicate("\tprintf(\"%c %d\", c, i);"));
	arraylist_add(file, strduplicate("\ttest2(param);")); //L.10
	arraylist_add(file, strduplicate("\tfunction(c);"));
	arraylist_add(file, strduplicate("\tfunction(test2(test(c2)));"));
	arraylist_add(file, strduplicate("\tfunction(25);"));
	arraylist_add(file, strduplicate("\tchar *ret = test2(param, param3, 'c');"));
	arraylist_add(file, strduplicate("\tdeclared();"));
	arraylist_add(file, strduplicate("\tproto();"));
	arraylist_add(file, strduplicate("\tint size = sizeof(param);"));
	arraylist_add(file, strduplicate("\ttest2(c + a, b);"));
	arraylist_add(file, strduplicate("\treturn test2(&c, b);"));
	arraylist_add(file, strduplicate("}")); //L.20

	arraylist_add(file, strduplicate("int main() {"));
	arraylist_add(file, strduplicate("\tint i = 42;"));
	arraylist_add(file, strduplicate("\treturn j;"));
	arraylist_add(file, strduplicate("}"));

	arraylist_add(file, strduplicate("char* test2(char v) {"));
	arraylist_add(file, strduplicate("\tv = glob;"));
	arraylist_add(file, strduplicate("\tint wrong = \"wrong\";"));
	arraylist_add(file, strduplicate("\twrong = 'w';"));
	arraylist_add(file, strduplicate("\twrong = test2('w');"));
	arraylist_add(file, strduplicate("\twhile(i);"));
	arraylist_add(file, strduplicate("\twhile(j) {"));
	arraylist_add(file, strduplicate("\t\t//..."));
	arraylist_add(file, strduplicate("\t\tif(k);")); //L.30
	arraylist_add(file, strduplicate("\t\tif(l) {"));
	arraylist_add(file, strduplicate("\t\t\tbreak;"));
	arraylist_add(file, strduplicate("\t\t\tbreak();"));
	arraylist_add(file, strduplicate("\t\t}"));
	arraylist_add(file, strduplicate("\t\telse if(m) {"));
	arraylist_add(file, strduplicate("\t\t}"));
	arraylist_add(file, strduplicate("\t\telse {"));
	arraylist_add(file, strduplicate("\t\t\tif(v, k);"));
	arraylist_add(file, strduplicate("\t\t}"));
	arraylist_add(file, strduplicate("\t}")); //L.40
	arraylist_add(file, strduplicate("\t do {"));
	arraylist_add(file, strduplicate("\t}"));
	arraylist_add(file, strduplicate("\twhile(test);"));
	arraylist_add(file, strduplicate("\tswitch(p) {"));
	arraylist_add(file, strduplicate("\tcase 1: printf();"));
	arraylist_add(file, strduplicate("\tbreak;"));
	arraylist_add(file, strduplicate("\tcase 2: int i = p;"));
	arraylist_add(file, strduplicate("\tcase 1: case p: case i: test();"));
	arraylist_add(file, strduplicate("\tcase 1: case 2: default: test();"));
	arraylist_add(file, strduplicate("\tbreak;")); //L.50
	arraylist_add(file, strduplicate("\tcase test2():"));
	arraylist_add(file, strduplicate("\tbreak;"));
	arraylist_add(file, strduplicate("\tdefault: printf(\"default\");"));
	arraylist_add(file, strduplicate("\t}"));
	arraylist_add(file, strduplicate("\ttest2(\"/* comment */\") /* test */;"));
	arraylist_add(file, strduplicate("\treturn;"));
	arraylist_add(file, strduplicate("\treturn 'c';"));
	arraylist_add(file, strduplicate("\treturn 'd' + p;"));
	arraylist_add(file, strduplicate("}"));

	scope_t *scope = parse_root_scope(file);
	if(scope != NULL) {
		print_scope(scope, 0);
		printf("Return: %d\n", parse_and_check(scope, file, NULL, NULL));
		scope_free(scope);
	} else {
		printf("%sScope %sNULL\n%s", COLOR_YELLOW, COLOR_RED, FORMAT_RESET);
	}

	arraylist_free(file, 1);

}

static void test_operation(char* line, unsigned int line_index, scope_t *scope) {
	printf("%sInput: %s\"%s\"%s\n", COLOR_BLUE, COLOR_YELLOW, line, FORMAT_RESET);

	messages_t *messages = malloc(sizeof(messages_t));
	messages->undeclared_functions = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	messages->undeclared_variables = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	messages->invalid_params       = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	messages->invalid_calls        = NULL;
	messages->variables_list       = NULL;
	messages->functions_list       = NULL;
	messages->wrong_assignment     = NULL;
	type_t type = parse_operation(line, line_index, scope, messages);
	printf("%sOutput: %s\n", COLOR_BLUE, FORMAT_RESET);
	printf("\t%sType:       %s%s\n", COLOR_CYAN, FORMAT_RESET, type.name);
	printf("\t%sIs pointer: %s%d\n", COLOR_CYAN, FORMAT_RESET, type.is_pointer);
	
	free(type.name);

	for(size_t i = 0 ; i < messages->undeclared_functions->size ; i++) {
		printf("\t%sUndefined function: %s%s\n", COLOR_RED, FORMAT_RESET, ((function_t*)arraylist_get(messages->undeclared_functions, i))->name);
	}

	for(size_t i = 0 ; i < messages->undeclared_variables->size ; i++) {
		printf("\t%sUndefined variable: %s%s\n", COLOR_RED, FORMAT_RESET, (char*)arraylist_get(messages->undeclared_variables, i));
	}

	for(size_t i = 0 ; i < messages->invalid_params->size ; i++) {
		printf("\t%sInvalid param: %s%s\n", COLOR_RED, FORMAT_RESET, ((field_t*)arraylist_get(messages->invalid_params, i))->name);
	}

	function_list_free(messages->undeclared_functions);
	field_list_free(messages->invalid_params);
	arraylist_free(messages->undeclared_variables, 1);
	free(messages);
}

static void test_parsing_operations() {
	
	printf("------------------------------%s\n", FORMAT_RESET);
	printf("%sTESTING PARSE OPERATION%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	arraylist_t *file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("static int glob = 89;"));
	arraylist_add(file, strduplicate("static int a = 97;"));
	arraylist_add(file, strduplicate("static int **d;"));
	arraylist_add(file, strduplicate("static unsigned char b = 98;"));
	arraylist_add(file, strduplicate("static unsigned char ***ptr;"));
	arraylist_add(file, strduplicate("int *ptr1;"));
	arraylist_add(file, strduplicate("int *ptr2;"));
	arraylist_add(file, strduplicate("char* test2(char v) {"));
	arraylist_add(file, strduplicate("\tv = glob;"));
	arraylist_add(file, strduplicate("}"));

	scope_t *scope = parse_root_scope(file);

	test_operation("int i = 0;", 9, scope);
	test_operation("a + b", 9, scope);
	test_operation("b + a", 9, scope);
	test_operation("ab += ba", 9, scope);
	test_operation("a << b", 9, scope);
	test_operation("a <<= ba + ca", 9, scope);
	test_operation("a * b", 9, scope);
	test_operation("a * *ptr", 9, scope);
	test_operation("*ptr * a", 9, scope);
	test_operation("a & &ptr", 9, scope);
	test_operation("&ptr & a", 9, scope);
	test_operation("***ptr * **d", 9, scope);
	test_operation("***ptr***d", 9, scope);
	test_operation("a *", 9, scope);
	test_operation("a <<= b < c << e;", 9, scope);
	test_operation("a <<< b", 9, scope);
	test_operation("test(e) + 12.8646f;", 8, scope);
	test_operation("a = b + test(e) + 12.8646f;", 8, scope);
	test_operation("1 / 2f", 9, scope);
	test_operation("/* comment */ 1 / 2f", 9, scope);
	test_operation("/* comment */ 1 / 2f /*comment*/", 9, scope);
	test_operation("//comment\n1d + 3", 9, scope);
	test_operation("1d //comment\n * 3", 9, scope);
	test_operation("1d * 3", 9, scope);
	test_operation("ptr1 + ptr2", 9, scope);
	test_operation("ptr1 + 1", 9, scope);
	test_operation("ptr1 - 1", 9, scope);
	test_operation("1 - ptr1", 9, scope);
	test_operation("1 + ptr1", 9, scope);
	test_operation("ptr1 - ptr2", 9, scope);
	test_operation("b < 88.0", 9, scope);
	test_operation("b <= 88.0", 9, scope);
	test_operation("b >= 88.0", 9, scope);
	test_operation("b == 88.0", 9, scope);
	test_operation("b != 88.0", 9, scope);
	test_operation("b ! 88.0", 9, scope);
	test_operation("!*&b", 9, scope);

	scope_free(scope);
	arraylist_free(file, 1);
}

static void test_rule_no_multi_declaration() {

	printf("------------------------------%s\n", FORMAT_RESET);
	printf("%sTESTING RULE: no-multi-declaration%s\n", COLOR_GREEN_BOLD, FORMAT_RESET);

	arraylist_t *file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_add(file, strduplicate("int glob = 89;"));
	arraylist_add(file, strduplicate("int a = 89, b = 42;"));
	arraylist_add(file, strduplicate("char c = 'a', b = 'd', k;"));
	arraylist_add(file, strduplicate("int func() {"));
	arraylist_add(file, strduplicate("\tshort var = 4;"));
	arraylist_add(file, strduplicate("\tshort var2 = 5, var3 = 6;"));
	arraylist_add(file, strduplicate("}"));

	arraylist_add(file, strduplicate("void params(int param1, char param2) {"));
	arraylist_add(file, strduplicate("}"));

	scope_t *scope = parse_root_scope(file);

	if(scope != NULL) {
		printf("Return: %d\n", check_no_multi_declaration(scope, file));
		scope_free(scope);
	} else {
		printf("%sScope %sNULL\n%s", COLOR_YELLOW, COLOR_RED, FORMAT_RESET);
	}

	arraylist_free(file, 1);

}

void test() {

	test_variable_parsing();
	test_function_parsing();
	test_scope_parsing();
	test_rule_no_prototype();
	test_function_call_parsing();
	test_parse_expression();
	test_parsing_operations();
	test_rule_no_multi_declaration();
	test_rule_parsing();

	printf("------------------------------%s\n", FORMAT_RESET);
}
