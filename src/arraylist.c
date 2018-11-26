#include "arraylist.h"

#define ARRAYLIST_GROW_FACTOR 1.5

static void arraylist_set_null(arraylist_t *list) {
	for(size_t i = list->size ; i < list->capacity ; i++) {
		list->array[i] = NULL;
	}
}

arraylist_t *arraylist_init(unsigned int initial_capacity) {
	arraylist_t *list = malloc(sizeof(arraylist_t));
	
	if(list != NULL) {
		list->size = 0;
		list->capacity = initial_capacity;
		list->array = malloc(list->capacity * sizeof(void*));
		if(list->array == NULL) {
			free(list);
			list = NULL;
		}
		arraylist_set_null(list);
	}
	return list;
}

static void arraylist_grow(arraylist_t *list) {
	list->capacity *= ARRAYLIST_GROW_FACTOR;
	list->array = realloc(list->array, list->capacity * sizeof(void*));
	arraylist_set_null(list);
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

void *arraylist_get(arraylist_t *list, size_t index) {
	return index >= list->size ? NULL : list->array[index];
}

void *arraylist_remove(arraylist_t *list, size_t index) {
	void *value = arraylist_get(list, index);
	size_t num_moved;

	if(value == NULL) return NULL;

	num_moved = list->size - index;
	if(num_moved > 0) {
		for(size_t i = 1 ; i <= num_moved ; i++) {
			list->array[index + i - 1] = list->array[index + i];
		}
	}
	list->array[--(list->size)] = NULL;
	return value;
}

void arraylist_free(arraylist_t *list, unsigned char free_content) {
	if(free_content) {
		for(unsigned int i = 0 ; i < list->size ; i++) {
			if(list->array[i] != NULL)
				free(list->array[i]);
		}
	}
	free(list->array);
	free(list);
}
