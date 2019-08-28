

#include <stdlib.h>
#include <stdio.h>

#include "lists.h"

int main()
{

    printf("sizeof void** = %I64u\n", sizeof(void **));
    struct GrowableList *exa = growable_list_create(3);

    printf("A new growable list has been created, located at %p (max_size=%I64u)\n", &exa, exa->max_size);

    for (size_t i = 0; i < 50; i++)
    {

        growable_list_add_element(exa, malloc(16));
        // printf("new element added, current size=%I64u/%I64u\n", exa->size, exa->max_size);
    }

    growable_list_print(exa);
}