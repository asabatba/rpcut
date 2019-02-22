
#include <stdint.h>

#include "args.h"

typedef uint16_t hash_t;
typedef uint32_t oid_t;

struct Buffer
{
  char *buffer;
  char *cur;
  size_t size;
};

typedef struct Ref
{
  char *start;
  uint32_t length; // ref max size??

  struct Element *to;   // formerly element
  struct Element *from; // formerly parent

  struct Ref *next_to;   // formerly just next
  struct Ref *next_from; // did not exist

  int8_t removed;
  // struct Element *parent;
} Ref;

// IdList *subindex_idl[256];

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

enum attribute_type
{
  ID,
  PARENT_ID,
  NAME,
  OTHER
};

// enum EleType
// {
//   UNDEFINED,
//   DATABASE,
//   PHYSICAL_TABLE,
//   PHYSICAL_COLUMN,
//   CONNECTION_POOL,
//   VARIABLE,
//   SCHEMA,
//   INIT_BLOCK,
//   BUSINESS_MODEL,
//   LOGICAL_TABLE,
//   LOGICAL_COLUMN,
//   PRESENTATION_CATALOG,
//   PRESENTATION_TABLE,
//   PRESENTATION_COLUMN,
//   PRESENTATION_HIERARCHY,
//   PRESENTATION_LEVEL,
//   USER,
//   GROUP

// };

// struct EleTypeRel {
//   enum EleType code;
//   char name[32];
// };


struct Buffer *file_to_buffer(const char *filename);

int parse_header(struct Buffer *buf, FILE *oxml);

char *get_next_tag(struct Buffer *source);

Element *tag_parse(char *raw, char **applist);

void ele_decider();

void save_xml(FILE *oxml, Element *first_tag, struct Buffer *input_buffer);

void html_result(struct t_args);

void funspeed_print();

// loop busca relaciones entre elementos
void logical_table_source_loop();