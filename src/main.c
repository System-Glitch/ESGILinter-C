#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "conf.h"
#include "display.h"
#include "fileloader.h"
#include "test.h"

#include "scopetree.h"
#include "rules/no_prototype.h"
#include "rules/no_multi_declaration.h"
#include "rules/parsing.h"
#include "rules/max_file_line_numbers.h"
#include "rules/max_line_numbers.h"
#include "rules/no_trailing_spaces.h"
#include "rules/comma_spacing.h"

static arraylist_t *get_lines_list(arraylist_t *buffer) {
	arraylist_t *lines = arraylist_init(buffer->size);

	for(size_t i = 0 ; i < buffer->size ; i++) {
		arraylist_add(lines, strduplicate(((line_t*)arraylist_get(buffer, i))->line));
	}

	return lines;
}

int main(int argc, char **argv) {
	char *path = NULL;
	char *file = NULL;
	arraylist_t *conf = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_t *files = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
	arraylist_t *buffer = NULL;
	arraylist_t *real_file = NULL;
	arraylist_t *lines = NULL;
	scope_t *scope = NULL;
	rule_t *rule = NULL;

	if(conf == NULL || files == NULL) {
		printf("%s[ERROR]%s %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
		return EXIT_FAILURE;
	}

	path = malloc(sizeof(char) * 255);
	strcpy(path, "."); // replace with an argument

	if(load_configuration("linter.conf", conf)) { //replace with argument
		printf("%s[ERROR]%s Failed to load config: %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
		arraylist_free(conf, 0);
		return EXIT_FAILURE;
	}

	search_files(conf, files, path);

	for(int i = 0; i < files->size; i++){
		buffer = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);
		real_file = arraylist_init(ARRAYLIST_DEFAULT_CAPACITY);

		if(buffer == NULL || real_file == NULL) {
			printf("%s[ERROR]%s %s%s\n", COLOR_RED, COLOR_YELLOW, strerror(errno), FORMAT_RESET);
			return EXIT_FAILURE;
		}

		file = arraylist_get(files, i);
		file_loader(buffer, files, real_file, file);

		printf("Checking file: %s\n", file);
		file_loader(buffer, files, real_file, file);
		lines = get_lines_list(buffer);
		scope = parse_root_scope(lines);
		arraylist_free(lines, 1);

		if(scope != NULL) {
			if(check_rule(conf, "max-file-line-numbers")) {
				rule = get_rule(conf, "max-file-line-numbers");
				max_file_line_numbers(real_file, rule->value, file); //TODO handle includes
			}
			if(check_rule(conf, "max-line-numbers")) {
				rule = get_rule(conf, "max-line-numbers");
				check_max_line_length(real_file, rule->value, file); //TODO handle includes
			}
			if(check_rule(conf, "no-trailing-space")) {
				no_trailing_spaces(real_file, file); //TODO handle includes
			}
			if(check_rule(conf, "comma-spacing")) {
				comma_spacing(buffer, real_file);
			}
			if(check_rule(conf, "no-prototype")) {
				check_no_prototype(scope, buffer, real_file);
			}
			if(check_rule(conf, "no-multi-declaration")) {
				check_no_multi_declaration(scope, buffer, real_file);
			}
			parse_and_check(scope, buffer, NULL, NULL, conf, real_file);
		} else {
			printf("%s[WARNING]%s Scope parsing failed for %s%s\n", COLOR_RED, COLOR_YELLOW, FORMAT_RESET, file);
		}

		/*
		 *
		 * TREAT DATA FROM HERE
		 *
		 * APPLY RULES
		 *
		 */
		arraylist_free(real_file, 1);
		free_buffer(buffer);
	}
	arraylist_free(files, 1);

	free_conf(conf);
    free(path);
	return EXIT_SUCCESS;
}
