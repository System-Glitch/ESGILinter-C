#include "scopetree.h"
#include "parsing_operations.h"
#include "parsing_variables.h"

static const char *known_operators[] = {
	"==","<<=",">>=","<=",">=","!=","+=",
	"-=","*=","/=","%=","&=","^=","|=","=",
	"+","-","/","*","%","<<",">>","~",
	"<",">","&&","||","&","|","^",
	NULL
}; //Order is important to avoid skipping the end of the operator (< and <= for example)

static const char *comparison_operators[] = {
	"==","<=",">=","!=","<",">","&&","||",
	NULL
};

static const char *type_rank[] = {
	"void", "char", "unsigned char",
	"short", "unsigned short",
	"int", "size_t", "unsigned int",
	"long", "unsigned long",
	"long long", "unsigned long long",
	"long long int", "unsigned long long int",
	"float", "double", "long double",
	NULL
};

static unsigned char is_comparison(const char *operator) {
	int i = 0;
	const char *t = NULL;
	while((t = comparison_operators[i]) != NULL) {
		if(!strcmp(comparison_operators[i], operator))
			return 1;
		i++;
	}
	return 0;
}

static int find_type_rank(char *type) {
	int i = 0;
	const char *t = NULL;
	while((t = type_rank[i]) != NULL) {
		if(!strcmp(type_rank[i], type))
			return i;
		i++;
	}
	return -1;
}

static int get_highest_rank(char *type1, char *type2) {

	if(!strcmp(type1, "NULL") || !strcmp(type2, "NULL"))
		return -1;

	int rank1 = find_type_rank(type1);
	int rank2 = find_type_rank(type2);
	return rank1 > rank2 ? rank1 : rank2;
}

static type_t parse_operand(char *operand, int line_index, scope_t *scope, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params, arraylist_t *variables_list, arraylist_t *functions_list, arraylist_t *invalid_calls) {
	type_t type = parse_operation(operand, line_index, scope, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);

	if(!strcmp(type.name, "NULL")) {
		free(type.name);
		type = parse_expression(operand, line_index, scope, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);
	}

	return type;
}

static unsigned char is_operator_first(char *line, int length, char *occurrence) {
	int index = 0;
	char c;

	SKIP_WHITESPACES

	return occurrence == line + index; 
}

type_t parse_operation(char *line, int line_index, scope_t *scope, arraylist_t *undeclared_variables, arraylist_t *undeclared_functions, arraylist_t *invalid_params, arraylist_t *variables_list, arraylist_t *functions_list, arraylist_t *invalid_calls) {
	type_t type;
	type_t left_operand_type;
	type_t right_operand_type;
	type_t *ptr_type;
	scope_t *root_scope       = get_root_scope(scope);
	char *line_wo_comment     = str_remove_comments(line);
	char *tmp_line            = NULL;
	char *left_operand        = NULL;
	char *right_operand       = NULL;
	arraylist_t *declarations = NULL;
	const char *operator      = NULL;
	char *occurrence          = NULL;
	int   index_operator      =  0;
	int   left_operand_length = -1;
	int   right_operand_index = -1;
	int   length              = strlen(line_wo_comment);
	int   operator_length     = -1;
	int   rank                = -1;
	char  is_declaration      =  0;

	type.name = strduplicate("NULL");
	type.is_pointer = 0;
	type.is_literal = 0;

	if(strindexof(line, ':') != -1) {
		return type; //TODO temp fix for case and ternary operator
	}

	while((operator = known_operators[index_operator++]) != NULL) { //Find operator
		tmp_line = line_wo_comment;
		operator_length = strlen(operator);
		occurrence = strstr(line_wo_comment, operator);
		if(occurrence != NULL) {

			//Check if inside quotes
			if(check_quotes(line_wo_comment, occurrence, length))
				continue;

			if(!strcmp(operator, "*") || !strcmp(operator, "&")) {

				//Check if not function declaration
				for(size_t i = 0 ; i < root_scope->functions->size ; i++) {
					if(((function_t*)arraylist_get(root_scope->functions, i))->line == line_index) {
						free(line_wo_comment);
						return type;
					}
				}

				while(is_operator_first(tmp_line, length, occurrence)) {
					tmp_line = occurrence + operator_length;
					occurrence = strstr(tmp_line, operator);
					if(occurrence == NULL)
						break;
				}
			} else if(is_operator_first(tmp_line, length, occurrence)) {
				free(line_wo_comment);
				return type;
			}

			if(occurrence == NULL) continue;

			//Split
			left_operand_length = occurrence - line_wo_comment;
			right_operand_index = left_operand_length + strlen(operator);
			left_operand        = strsubstr(line_wo_comment, 0, left_operand_length);
			right_operand       = strsubstr(line_wo_comment, right_operand_index, length - right_operand_index);

			//TODO Handle parenthesis

			//If left operation is variable declaration
			declarations = get_variables_from_declaration(line_index, left_operand);
			if(declarations != NULL && declarations->size > 0) {
				//Is a declaration, no need to parse expression

				free(type.name);
				type.name = strduplicate(((field_t*)arraylist_get(declarations, 0))->type.name);

				field_list_free(declarations);
				is_declaration = 1;
			} else {
				left_operand_type = parse_operand(left_operand, line_index, scope, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);
				is_declaration = 0;
			}

			right_operand_type = parse_operand(right_operand, line_index, scope, undeclared_variables, undeclared_functions, invalid_params, variables_list, functions_list, invalid_calls);

			if(!is_declaration && strcmp(left_operand_type.name,"NULL") && strcmp(right_operand_type.name,"NULL")) {

				if(is_comparison(operator)) { //Type is int
					free(type.name);
					type.name = strduplicate("int");
					type.is_pointer = 0;
				} else if(right_operand_type.is_pointer || left_operand_type.is_pointer) {
					ptr_type = right_operand_type.is_pointer > left_operand_type.is_pointer ? &right_operand_type : &left_operand_type;
					free(type.name);
					type.name = strduplicate(ptr_type->name);
					type.is_pointer = ptr_type->is_pointer;
				} else {
					rank = get_highest_rank(right_operand_type.name, left_operand_type.name);
					if(rank != -1) {
						free(type.name);
						type.name = strduplicate((char*)type_rank[rank]);
						type.is_pointer = 0;
					}
				}

			}

			free(right_operand_type.name);
			if(!is_declaration)
				free(left_operand_type.name);
			//TODO detect forbidden operations (2 * &ptr , void + something, int - ptr)

			//TODO Operand case ignored : int test[] = {length, 4};
			break;
		}
	}

	free(line_wo_comment);

	return type;	
}