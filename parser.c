
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "parser.h"

size_t xml_first_pass(struct GrowableList *element_list, struct Buffer *xml_buffer)
{
    if (element_list == NULL || xml_buffer == NULL)
        return NULL;

    for (size_t i = 0;; i++)
    {
        rawtag = find_next_tag(input_buffer);
        if (!rawtag) // EOF!
        {
            break;
        }

        itag = tag_parse(rawtag, arglist.apps);

        // mostrador de progreso
        if (i % load_limit == 0)
        {
            load_limit = load_limit * 2 - 1;
            printf(".");
        }

        if (i == 0)
        {
            first_tag = itag;
        }
        else
        {
            prev_tag->next_tag = itag;
        }
        prev_tag = itag;
    }
}

char *find_next_tag(struct Buffer *source)
{

    char closing_tag[MAX_TAG_NAME] = {0};

    strcpy(closing_tag, "</"); //snprintf ??

    char *start, *cur;

    start = strchr(source->cur, '<');

    cur = copy_until(closing_tag + 2, cur, " >");
    strcat(closing_tag, ">");

    snprintf(closing_tag, MAX_TAG_NAME, "</%s", cur);
}
