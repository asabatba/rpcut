#pragma once

struct GrowableList
{
    void **array;
    size_t size;
    size_t max_size;
};

struct GrowableList *growable_list_create(size_t initial_size);

size_t growable_list_grow(struct GrowableList *list);

size_t growable_list_add_element(struct GrowableList *list, void *new_element);

void growable_list_print(struct GrowableList *list);
