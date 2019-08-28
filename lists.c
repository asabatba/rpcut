

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "lists.h"

#define GROW_FACTOR 10
#define GROWABLE_LIST_MAX_ABSOLUTE_SIZE 1000 * 1000 * 1000

struct GrowableList *growable_list_create(size_t initial_size)
{

    struct GrowableList *out = malloc(sizeof(struct GrowableList));

    if (initial_size < 1)
    {
        out->array = NULL;
        out->max_size = 0;
        out->size = 0;
    }
    else
    {
        out->array = calloc(initial_size, sizeof(void *));
        out->max_size = initial_size;
        out->size = 0;
    }
    return out;
};

size_t growable_list_grow(struct GrowableList *list)
{

    size_t new_max_size = list->max_size * GROW_FACTOR;
    assert(new_max_size <= GROWABLE_LIST_MAX_ABSOLUTE_SIZE);

    printf("Will try to realloc list from %I64ub to %I64ub\n", list->max_size, new_max_size);

    list->array = realloc(list->array, new_max_size * sizeof(void *));
    assert(list->array); // assert we did not run out of memory
    list->max_size = new_max_size;

    return new_max_size;
}

size_t growable_list_add_element(struct GrowableList *list, void *new_element)
{

    if (new_element == NULL)
    {
        // huh, we're getting a NULL new element
        return 0;
    }

    if (list->size == list->max_size)
    {
        growable_list_grow(list);
    }

    list->array[list->size] = new_element;
    list->size++;

    return list->size;
};

void growable_list_print(struct GrowableList *list)
{

    for (size_t i = 0; i < list->size; i++)
    {
        printf("%I64u -> %p\n", i, list->array[i]);
    }
};
