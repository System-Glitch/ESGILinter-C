#include "scopetree.h"
#include "parsing_functions.h"
#include "parsing_expressions.h"

static void parse_variable_expression(char *line, char **name, char *is_pointer) {
	unsigned int start_index = 0;
	unsigned int end_index   = 0;
	int index                = 0;
	int length               = strlen(line);
	char c;

	//Pre incrementing / decrementing
	SKIP_WHITESPACES
	if(strstr(line + index, "++") == line + index || strstr(line + index, "--") == line + index) {
		//Skip operation
		index++;	
	} else if(index >= 0) {
		index--;
		c = line[index];
	}

	//Check referencing
	do {
		index++;
		SKIP_WHITESPACES

		if(c == '*') (*is_pointer)--;
		if(c == '&') (*is_pointer)++;
	} while(c == '*' || c == '&');

	SKIP_WHITESPACES

	start_index = index;

	//Until not alphanumeric
	while(is_alphanumeric(c = line[index]) && index < length) {
		index++;
	}

	end_index = index;

	//Check if name is followed by anything
	SKIP_WHITESPACES
	if(index < length && c != ';' && !is_whitespace(c)) {
		return;
	}

	if(end_index != start_index)
		*name = strsubstr(line, start_index, end_index - start_index);

}

static unsigned char is_type_identifier(char c) {
	return c == 'u' || c == 'l' || c == 'f' || c == 'U' || c == 'L' || c == 'F' || c == 'd' || c == 'D'; 
}

static void parse_float_literal(char *line, int length, int index, type_t *type) {
	char *type_identifier = NULL;
	char c = line[++index];
	if(is_digit(c)) {
		while(is_digit(c = line[index]) && index < length) {
			index++;
		}

		if(index >= length) {
			type_identifier = "double";
		} else if(is_type_identifier(c)) {
			switch(c) {
				case 'f':
				case 'F':
				type_identifier = "float";
				break;
				case 'd':
				case 'D':
				type_identifier = "double";
				break;
				case 'l':
				case 'L':
				type_identifier = "long double";
				break;
			}

		}

		if(type_identifier != NULL) {
			free(type->name);
			type->name = strduplicate(type_identifier);
		}
	}
}

static void parse_integer_literal(char *line, int length, int index, type_t *type) {
	char *tmp             = NULL;
	char *type_identifier = NULL;
	char c = line[index];

	while(is_type_identifier(c) && index < length) {

			switch(c) {
				case 'u':
				case 'U':
				type_identifier = !strcmp(type->name, "NULL") ? "unsigned" : "unsigned ";
				break;
				case 'l':
				case 'L':
				type_identifier = !strcmp(type->name, "NULL") ? "long" : " long";
				break;
				case 'f':
				case 'F':
				type_identifier = !strcmp(type->name, "NULL") ? "float" : " float";
				break;
				case 'd':
				case 'D':
				type_identifier = !strcmp(type->name, "NULL") ? "double" : " double";
				break;
			}

			if(!strcmp(type->name, "NULL")) {
				free(type->name);
				type->name = strduplicate(type_identifier);
			} else {
				
				//If unsigned, float or double already set, wrong
				if(!strcmp(type_identifier, "unsigned "))
					tmp = strconcat(type_identifier, type->name);
				else
					tmp = strconcat(type->name, type_identifier);
				free(type->name);
				type->name = tmp;
			}

			if(index < length)
				c = line[++index];
		}

		if(index < length && c != ';' && !is_whitespace(c)) { //Syntax error
			free(type->name);
			type->name = strduplicate("NULL");
		} else if(!strcmp(type->name, "unsigned")) {
			//If just "unsigned", add "int" at the end
			tmp = strconcat(type->name, " int");
			free(type->name);
			type->name = tmp;
		}
}

static void parse_number_literal(char *line, int length, int index, type_t *type) {
	char c;

	//Get all digits
	while(is_digit(c = line[index]) && index < length) {
		index++;
	}

	if(c == '.') { //Float or double
		parse_float_literal(line, length, index, type);
	} else if(is_type_identifier(c)) {
		parse_integer_literal(line, length, index, type);
	} else {
		type->name = strduplicate("int");
		type->is_literal = 1;
		type->is_pointer = 0;
	}
}

static void check_function_expression(function_t *function, char *line, int line_index, scope_t *scope, type_t *type, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params, arraylist_t *variables_list, arraylist_t *functions_list, arraylist_t *invalid_calls) {
	function_t *function_dec = NULL;
	function_t *prototype    = NULL;

	if(scope == NULL)
		arraylist_add(undeclared_functions, function);
	else {

		function_dec = find_function(scope, function->name, 0);
		if(function_dec != NULL && (function_dec->line < line_index)) {
			prototype = find_function_prototype(get_root_scope(scope), function->name);
			if(function_dec->line < line_index || (prototype != NULL && prototype->line < line_index)) {
				free(type->name);
				type->name = strduplicate(function_dec->return_type.name);
				type->is_pointer = function_dec->return_type.is_pointer;
				type->is_literal = 0;
				if(functions_list) {
					arraylist_remove(functions_list, arraylist_index_of(functions_list, function_dec));
				}
				check_function_call_parameters(scope, function, function_dec, line_index, line, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);
				function_free(function);
			} else {
				arraylist_add(undeclared_functions, function);
				check_function_call_parameters(scope, function, function_dec, line_index, line, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);
			}
		} else {
			arraylist_add(undeclared_functions, function);
			check_function_call_parameters(scope, function, function_dec, line_index, line, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);
		}
	}
}

static void check_variable_expression(char *line, int index, int line_index, scope_t *scope, type_t *type, arraylist_t *undeclared_variables, arraylist_t *variables_list) {
	field_t *variable_dec = NULL;
	char *variable_name   = NULL;
	char is_pointer       = 0;

	parse_variable_expression(line + index, &variable_name, &is_pointer);
	if(variable_name != NULL && !is_keyword(variable_name)) {

		variable_dec = find_variable(scope, variable_name);
		if(scope == NULL || variable_dec == NULL || variable_dec->line > line_index)
			arraylist_add(undeclared_variables, variable_name);
		else {
			if(variables_list) 
				arraylist_remove(variables_list, arraylist_index_of(variables_list, variable_dec));
			type->is_pointer = variable_dec->type.is_pointer + is_pointer;
			if(type->is_pointer < 0) { //More dereferencing than allowed
				type->is_pointer = 0;
				type->is_literal = 0;
			} else {
				type->name = strduplicate(variable_dec->type.name);
				type->is_literal = 0;
			}
		}
	}
}

static char *parse_control(char *line, int index, int length) {
	static const char *simple_controls[] = {
		"if", "else if", "switch", "while", NULL
	};
	//Case, default
	// : operator
	//For
	//Return
	const char *word   = NULL;
	size_t i = 0;
	char c;
	int last_index = -1;

	while((word = simple_controls[i++]) != NULL) {
		if(strstr(line + index, word) == line + index) {
			index += strlen(word);
			c = line[index];

			SKIP_WHITESPACES

			if(c == '(') {
				last_index = strlastindexof(line, ')');
				if(last_index > index) {
					index++;
					return strsubstr(line, index, last_index - index);
				}
			}
			break;
		}
	}
	return NULL;
}

type_t parse_expression(char *line, int line_index, scope_t *scope, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params, arraylist_t *variables_list, arraylist_t *functions_list, arraylist_t *invalid_calls) {
	type_t type;
	function_t *function     = NULL;
	char       *expr         = NULL;
	unsigned int start_index = 0;
	unsigned int end_index   = 0;
	unsigned int close_index = 0;
	int index                = 0;
	unsigned int sub_index   = 0;
	int length               = strlen(line);
	int type_length;
	char c;

	type.name = strduplicate("NULL");
	type.is_pointer = 0;
	type.is_literal = 0;

	SKIP_WHITESPACES

	if(c == '(') {
		//Parenthesis -> cast

		index++;

		SKIP_WHITESPACES

		start_index = index;

		while((c = line[index]) != ')' && index < length) index++;

		if(c != ')') { //No closing parenthesis, syntax error
			return type;
		}

		close_index = index;

		index++;

		//Check if something follows
		SKIP_WHITESPACES
		
		if(index >= length || is_whitespace(c) || c == ')') {
			return type;
		}

		index = close_index;

		//Skip whitespaces backwards to find end of type
		while(is_whitespace(c = line[index]) && index > 0) { index--; }

		end_index = index;

		free(type.name);
		type_length     = end_index - start_index;
		type.name       = strsubstr(line, start_index, type_length);
		type.is_pointer = strcount(type.name, '*');
		sub_index       = strcountuntil(type.name, '*', 1, 1);

		//Remove stars
		type.name[type_length - sub_index] = '\0';

		//Check expression without cast
		expr = strsubstr(line, close_index + 1, length - close_index);
		parse_expression(expr, line_index, scope, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);
		free(expr);


	} else if(c == '\'' && strlastindexof(line, '\'') != index) {
		free(type.name);
		type.name       = strduplicate("char");
		type.is_pointer = 0;
		type.is_literal = 1;
	} else if(c == '"' && strlastindexof(line, '"') != index) {
		free(type.name);
		type.name       = strduplicate("char");
		type.is_pointer = 1;
		type.is_literal = 1;
	} else {

		expr = parse_control(line, index, length);
		if(expr != NULL) {
			printf("CONTROL : %s\n", line+index);
			parse_expression(expr, line_index, scope, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);
		} else if(is_digit(c) || c == '.') {
			parse_number_literal(line, length, index, &type);
		} else if(undeclared_variables != NULL) {
			function = parse_function_call(line_index, line + index);
			if(function != NULL) {
				check_function_expression(function, line, line_index, scope, &type, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);
			} else {
				check_variable_expression(line, index, line_index, scope, &type, undeclared_variables, variables_list);
			}
		}

	}

	//TODO conditional expressions and loops (before variable and function expressions)
	//if, else if, for, switch, case, while, ternary

	return type;
}