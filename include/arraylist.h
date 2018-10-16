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
void         arraylist_add(arraylist_t *list, void *value);
void        *arraylist_get(arraylist_t *list, unsigned int index);
void         arraylist_free(arraylist_t *list);

#endif
