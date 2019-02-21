#include <stdint.h>


typedef uint16_t hash_t;
typedef uint32_t oid_t;

struct Buffer
{
  char *buffer;
  char *cur;
  size_t size;
};


struct Tag;

typedef struct Ref
{
  char *start;
  unsigned long length; // ref max size??

  struct Element *to;   // formerly element
  struct Element *from; // formerly parent

  struct Ref *next_to;   // formerly just next
  struct Ref *next_from; // did not exist

  char removed;
  // struct Element *parent;
} Ref;

// IdList *subindex_idl[256];

typedef struct Element
{
  unsigned long id;
  // struct Tag *tag;

  // inherited from Tag struct
  char *raw;
  unsigned long rawsize;

  char *tagname;
  char *appname;
  char *elename;

  char removal;
  char root;

  int16_t refcount;
  struct Ref *ref;    // elements that reference the CURRENT element
  struct Ref *ref_to; // elements that the current element references

  struct Element *next_tag; // next tag, as read from xml

  // struct Element *next; // next element in order of creation
  struct Element *hash_next;
  // struct Element *last;
  struct Element *hash_last;
  // unsigned long hast_size;

  struct Element *parent;
  struct Element *child;
  struct Element *sibling;

  // struct Element *logical_table_source;
  // struct Element *ref_physical_table;

  struct Element *physical;
  struct Element *logical;
  // struct Element *presentation;

} Element;


enum attribute_type
{
  ID,
  PARENT_ID,
  NAME,
  OTHER
};