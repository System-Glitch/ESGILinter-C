#include "scopetree.h"
#include "parsing_operations.h"
#include "parsing_variables.h"

static const char *known_operators[] = {
	"==","<<=",">>=","<=",">=","!=","+=",
	"-=","*=","/=","%=","&=","^=","|=","=",
	"+","-","/","*","%","<<",">>","~",
	"<",">","&&","||","!","&","|","^", 
	NULL
}; //Order is important to avoid skipping the end of the operator (< and <= for example)

static type_t parse_operand(char *operand, int line_index, scope_t *scope, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params) {
	type_t type = parse_operation(operand, line_index, scope, undeclared_variables, undeclared_functions, invalid_params);

	if(!strcmp(type.name, "NULL")) {
		free(type.name);
		type = parse_expression(operand, line_index, scope, undeclared_variables, undeclared_functions, invalid_params);
	}

	return type;
}

static unsigned char is_operator_first(const char *operator, char *line, int length, char *occurrence) {
	int index = 0;
	char c;

	SKIP_WHITESPACES

	return occurrence == line + index; 
}

type_t parse_operation(char *line, int line_index, scope_t *scope, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params) {
	type_t type;
	type_t left_operand_type;
	type_t right_operand_type;
	char *tmp_line            = NULL;
	char *left_operand        = NULL;
	char *right_operand       = NULL;
	arraylist_t *declarations = NULL;
	const char *operator      = NULL;
	char *occurrence          = NULL;
	int   index_operator      = 0;
	int   left_operand_length = -1;
	int   right_operand_index = -1;
	int   length              = strlen(line);
	int   operator_length     = -1;

	type.name = strduplicate("NULL");
	type.is_pointer = 0;
	type.is_literal = 0;

	while((operator = known_operators[index_operator++]) != NULL) { //Find operator
		tmp_line = line;
		operator_length = strlen(operator);
		occurrence = strstr(line, operator);
		if(occurrence != NULL) {

			if(!strcmp(operator, "*") || !strcmp(operator, "&")) {
				while(is_operator_first(operator, tmp_line, length, occurrence)) {
					tmp_line = occurrence + operator_length;
					occurrence = strstr(tmp_line, operator);
					if(occurrence == NULL)
						break;
				}
			} else if(is_operator_first(operator, tmp_line, length, occurrence)) {
				return type;
			}

			if(occurrence == NULL) continue;

			//Split
			left_operand_length = occurrence - line;
			right_operand_index = left_operand_length + strlen(operator);
			left_operand        = strsubstr(line, 0, left_operand_length);
			right_operand       = strsubstr(line, right_operand_index, length - right_operand_index);

			printf("Left: %s\n", left_operand); //TODO remove debug
			printf("Right: %s\n", right_operand);

			//If left operation is variable declaration
			declarations = get_variables_from_declaration(line_index, left_operand);
			if(declarations != NULL) {
				//Is a declaration, no need to parse expression
				field_list_free(declarations);
			} else {
				left_operand_type = parse_operand(left_operand, line_index, scope, undeclared_variables, undeclared_functions, invalid_params);
				printf("Left type: %s\n", left_operand_type.name);
				free(left_operand_type.name);
			}
			right_operand_type = parse_operand(right_operand, line_index, scope, undeclared_variables, undeclared_functions, invalid_params);
			printf("Right type: %s\n", right_operand_type.name);
			free(right_operand_type.name);

			free(type.name);
			type.name = strduplicate("OK");

			//TODO implicit return type of operation

			//Ignored case, parenthesis
			//Operand case ignored : int test[] = {length, 4};
			break;
		}
	}

	//If no operation, just function call, what should I do?

	return type;	
}