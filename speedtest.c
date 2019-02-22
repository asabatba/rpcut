// #pragma once

#include <stdlib.h>
#include <stdio.h>
// #include <time.h>

#include "speedtest.h"

#define MAX_FUNCTIONS 64 //num maximos de funciones

char *function_name_strings[] = {"get_next_tag", "tag_parse", "ele_merge", "ref_add"};

struct Funspeed
{
    // size_t index;
    struct Funspeed *next;
    // char *name;

    enum function_name function;

    clock_t clocks;
    size_t calls;
};

struct Funspeed *funspeed_idx[MAX_FUNCTIONS] = {0};

struct Funspeed *funspeed_create(enum function_name function)
{

    struct Funspeed *new = malloc(sizeof(struct Funspeed));

    new->function = function;
    new->clocks = 0;
    new->calls = 0;

    funspeed_idx[function] = new;

    return new;
};

void funspeed_update(enum function_name function, clock_t start, int debug)
{

    if (!debug)
        return;

    clock_t end = clock();

    struct Funspeed *fs = 0;

    if (funspeed_idx[function] == 0)
        fs = funspeed_create(function);
    else
        fs = funspeed_idx[function];

    fs->calls++;
    fs->clocks += end - start;

    return;
};

void funspeed_print_dbg(int debug)
{

    if (!debug)
        return;

    size_t i;
    struct Funspeed *ifun = 0;

    printf("Analisis de tiempos de procesado:\n");

    for (i = 0; i < MAX_FUNCTIONS; i++)
    {
        ifun = funspeed_idx[i];
        if (!ifun)
        {
            continue;
        }

        
        printf("funcion: %s\tllamadas: %lu\tclocks: %lu\n",
               function_name_strings[ifun->function],
               (long unsigned)ifun->calls, (long unsigned)ifun->clocks);
    }
    return;
}
