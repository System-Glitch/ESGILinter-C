#include "arraylist.h"

#define ARRAYLIST_GROW_FACTOR 1.5

arraylist_t *arraylist_init(unsigned int initial_capacity) {
	arraylist_t *list = malloc(sizeof(arraylist_t));
	
	if(list != NULL) {
		list->size = 0;
		list->capacity = initial_capacity;
		list->array = malloc(list->capacity * sizeof(void*));
		if(list->array == NULL) free(list);
	}

	return list;
}

static void arraylist_grow(arraylist_t *list) {
	list->capacity *= ARRAYLIST_GROW_FACTOR;
	list->array = realloc(list->array, list->capacity * sizeof(void*));
}

void arraylist_add(arraylist_t *list, void *value) {
	if(list->size == list->capacity) {
		arraylist_grow(list);
	}

	list->array[list->size++] = value;
}

void arraylist_add_all(arraylist_t *list, arraylist_t *values) {
	for(unsigned int i = 0 ; i < values->size ; i++)
		arraylist_add(list, arraylist_get(values, i));
}

void *arraylist_get(arraylist_t *list, unsigned int index) {
	return index >= list->size ? NULL : list->array[index];
}

void arraylist_free(arraylist_t *list) {
	for(unsigned int i = 0 ; i < list->size ; i++) {
		if(list->array[i] != NULL)
			free(list->array[i]);
	}
	free(list->array);
	free(list);
}
