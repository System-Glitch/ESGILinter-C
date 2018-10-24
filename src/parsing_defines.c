#include <stdlib.h>
#include <errno.h>
#include "parsing_defines.h"

static char *get_define_type(char *value) {
	//Substring the value (after last space after name, until new line)
	
	//If empty, type void
	//If not supported (macro, multiline), return NULL

	//TODO
	errno = ENOSYS;
	return NULL;
}

define_t *define_init(char *name, char *value) {
	define_t *define = malloc(sizeof(define_t));

	if(define != NULL) {
		define->name            = name;
		define->value           = value;
		define->type.name       = get_define_type(value);
		define->type.is_pointer = 0;
	}

	return define;
}

define_t *get_define_from_string(char *define) {
	//TODO
	errno = ENOSYS;
	return NULL;
}

void define_free(define_t *define) {
	free(define->name);
	free(define->value);
	free(define);
}
