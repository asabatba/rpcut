#pragma once

#include <inttypes.h>
#include "lists.h"
#include "strmsc.h"

typedef uint32_t oid_t; // oracle identifier (?)

typedef struct Ref
{
    char *start;
    uint32_t length;

    struct Element *to;
    struct Element *from;

    struct Ref *next_to;
    struct Ref *next_from;

    int8_t removed;

} Ref;

typedef struct Element
{
    oid_t id;

    char *raw;
    uint32_t rawsize;

    char *tagname;
    char *appname;
    char *elename;

    int8_t removal;
    int8_t root;

    int16_t refcount;
    struct Ref *ref;    // elements that reference the CURRENT element
    struct Ref *ref_to; // elements that the current element references

    struct Element *next_tag; // next tag, as read from xml

    struct Element *hash_next;
    struct Element *hash_last;

    struct Element *parent;
    struct Element *child;
    struct Element *sibling;

    struct Element *physical;
    struct Element *logical;

} Element;

/**
 * scans the provided xml_buffer and populates an element_list with all depth-1 elements
 **/
size_t xml_first_pass(struct GrowableList *element_list, struct Buffer *xml_buffer);
