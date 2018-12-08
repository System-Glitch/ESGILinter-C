#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include<stdlib.h>

#define ARRAYLIST_DEFAULT_CAPACITY 10

typedef struct arraylist {
	void   **array;
	size_t   size;
	size_t   capacity;
} arraylist_t;

arraylist_t *arraylist_init(unsigned int initial_capacity);
arraylist_t *arraylist_copy(arraylist_t *list);
void         arraylist_add(arraylist_t *list, void *value);
void         arraylist_add_all(arraylist_t *list, arraylist_t *values);
void        *arraylist_get(arraylist_t *list, size_t index);
void        *arraylist_remove(arraylist_t *list, size_t index);
size_t       arraylist_index_of(arraylist_t *list, void *value);
void         arraylist_free(arraylist_t *list, unsigned char free_content);

#endif
