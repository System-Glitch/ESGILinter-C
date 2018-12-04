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

static type_t parse_operand(char *operand, int line_index, scope_t *scope, messages_t *messages) {
	type_t type = parse_operation(operand, line_index, scope, messages);

	if(!strcmp(type.name, "NULL")) {
		free(type.name);
		type = parse_expression(operand, line_index, scope, messages);
	}

	return type;
}

static unsigned char is_operator_first(char *line, int length, char *occurrence) {
	int index = 0;
	char c;

	SKIP_WHITESPACES

	return occurrence == line + index; 
}

static void add_wrong_assignment_message(messages_t *messages, type_t *type_left, type_t *type_right) {
	wrong_type_t *assignment              = malloc(sizeof(wrong_type_t));
	assignment->expected_type             = malloc(sizeof(type_t));
	assignment->expected_type->name       = strduplicate(type_left->name);
	assignment->expected_type->is_pointer = type_left->is_pointer;
	assignment->actual_type               = malloc(sizeof(type_t));
	assignment->actual_type->name         = strduplicate(type_right->name);
	assignment->actual_type->is_pointer   = type_right->is_pointer;
	arraylist_add(messages->wrong_assignment, assignment);
}

static type_t find_variable_type_from_line(scope_t *scope, int line) {
	field_t *field = NULL;
	type_t type; //Default return;
	for(size_t i = 0 ; i < scope->variables->size ; i++) {
		field = arraylist_get(scope->variables, i);
		if(field->line == line) {
			return field->type;
		}
	}
	type.name = "NULL";
	return type;
}

static unsigned char starts_with_return(char *line, int length) {
	char c;
	int index = 0;

	SKIP_WHITESPACES

	return strstr(line + index, "return") == line + index;
}

static void parse_declarations(arraylist_t *declarations, scope_t *scope, int line_index, messages_t *messages) {
	field_t *declaration  = NULL;
	type_t declaration_type;

	for(size_t i = 0 ; i < declarations->size ; i++) {
		declaration = arraylist_get(declarations, i);
		if(declaration->value != NULL) {
			declaration_type = parse_expression(declaration->value, line_index, scope, messages);
			if(!strcmp(declaration_type.name, "NULL")) {
				declaration_type = parse_operation(declaration->value, line_index, scope, messages);
			}
			if(messages->wrong_assignment != NULL && strcmp(declaration_type.name,"NULL") && !type_equals(&declaration->type, &declaration_type)) {
				add_wrong_assignment_message(messages, &declaration->type, &declaration_type);
			}
			free(declaration_type.name);
		}
	}
}

type_t parse_operation(char *line, int line_index, scope_t *scope, messages_t *messages) {
	type_t type;
	type_t left_operand_type;
	type_t right_operand_type;
	type_t *ptr_type;
	scope_t *root_scope       = get_root_scope(scope);
	char *line_wo_comment     = str_remove_comments(line);
	arraylist_t *declarations = NULL;
	char *tmp_line            = NULL;
	char *left_operand        = NULL;
	char *right_operand       = NULL;
	const char *operator      = NULL;
	char *occurrence          = NULL;
	int   index_operator      =  0;
	int   left_operand_length = -1;
	int   right_operand_index = -1;
	int   length              = strlen(line_wo_comment);
	int   operator_length     = -1;
	int   rank                = -1;

	type.name = strduplicate("NULL");
	type.is_pointer = 0;
	type.is_literal = 0;

	if(strindexof(line, ':') != -1 || starts_with_return(line, length)) {
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

			//TODO Handle parenthesis

			//If left operation is variable declaration
			declarations = get_variables_from_declaration(line_index, line);
			if(declarations != NULL && declarations->size > 0) {
				parse_declarations(declarations, scope, line_index, messages);

				free(type.name);
				type.name = strduplicate(((field_t*)arraylist_get(declarations, 0))->type.name);

				field_list_free(declarations);

				//Find var declaration in scope
				left_operand_type = find_variable_type_from_line(scope, line_index);

			} else {
				right_operand      = strsubstr(line_wo_comment, right_operand_index, length - right_operand_index);
				left_operand_type  = parse_operand(left_operand, line_index, scope, messages);
				right_operand_type = parse_operand(right_operand, line_index, scope, messages);

				if(strcmp(left_operand_type.name,"NULL") && strcmp(right_operand_type.name,"NULL")) {

					if(messages->wrong_assignment != NULL && !strcmp(operator, "=") &&
						!type_equals(&left_operand_type, &right_operand_type)) {

						add_wrong_assignment_message(messages, &left_operand_type, &right_operand_type);
					}

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
				free(left_operand_type.name);
			}

			//TODO detect forbidden operations (2 * &ptr , void + something, int - ptr)

			//TODO Operand case ignored : int test[] = {length, 4};
			free(left_operand);
			free(right_operand);
			break;
		}
	}

	free(line_wo_comment);

	return type;	
}