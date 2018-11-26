#include "rules/no_multi_declaration.h"
#include "display.h"

static unsigned char list_contains(arraylist_t *list, int value) {
	for(size_t i = 0 ; i < list->size ; i++) {
		if(*(int*)arraylist_get(list, i) == value)
			return 1;
	}
	return 0;
}

unsigned int check_no_multi_declaration(scope_t *root_scope, arraylist_t *file) {
	node_t  *current     = NULL;
	field_t *variable    = NULL;
	int *tmp             = NULL;
	arraylist_t *lines   = arraylist_init(root_scope->variables->size);
	arraylist_t *ignores = arraylist_init(root_scope->variables->size);
	unsigned int count   = 0; //Counts warnings and errors

	for(size_t i = 0 ; i < root_scope->variables->size ; i++) {
		variable = (field_t*)arraylist_get(root_scope->variables, i);
		if(variable->line != -1 && !list_contains(ignores, variable->line)) {
			if(!list_contains(lines, variable->line)) {
				tmp = malloc(sizeof(int));
				*tmp = variable->line;
				arraylist_add(lines, tmp);
			} else {
				print_warning("fictive_file.c", variable->line, arraylist_get(file, variable->line), "Variable multiple declaration");
				tmp = malloc(sizeof(int));
				*tmp = variable->line;
				arraylist_add(ignores, tmp);
				count++;
			}
		}
	}

	arraylist_free(lines, 1);
	arraylist_free(ignores, 1);

	current = root_scope->children->head;
	while (current != NULL) {
		count += check_no_multi_declaration((scope_t*)current->val, file);
		current = current->next;
	}

	return count;
}