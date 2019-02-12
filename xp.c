#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "args.h"
#include "str_misc.h"

// #include <unistd.h>

// #include "args.h"

// #define MAX_STR_SIZE 128
#define MAX_ID_SIZE 16
#define MAX_TAG_NAME 32 // names like PhysicalColumn, etc
#define MAX_TAG_SIZE 1024
#define APP_NAME_SIZE 6 // caracteres que tiene una aplicacion
#define ELEMENT_NAME_SIZE 32
#define READ_BUF_SIZE 1024 * 1000         //bytes
#define WRITE_BUF_SIZE 1024 * 1000 * 1000 //1 MB

#define ELEMENT_HASH_SIZE 65536          // at least 256, has to fit in UNSIGNED INT, (max=65536; is great)
#define LOGICAL_TABLE_SOURCE_SIZE 131072 // size of list of all table sources

#define ELEMENT_HASH_MASK (ELEMENT_HASH_SIZE - 1)
typedef unsigned hash_t;

typedef unsigned long oid_t;

typedef struct Buffer
{
  char *buffer;
  FILE *stream;
  char *cursor;
  size_t index;
  size_t size;
  //size_t chars_read;
} Buffer;

//no need to destroy really
Buffer *file_to_buffer(char *filename)
{
  Buffer *b = malloc(sizeof(Buffer));

  FILE *f = fopen(filename, "rb");

  fseek(f, 0, SEEK_END);
  b->size = ftell(f);
  fseek(f, 0, SEEK_SET);

  b->buffer = malloc(b->size + 1);
  b->cursor = b->buffer;
  if (!b->buffer)
  {
    // ha fallado la asignacion de memoria para el fichero
    printf("malloc ha fallado para %s\n", filename);
  }
  fread(b->buffer, b->size, 1, f);
  fclose(f);

  b->index = 0;
  return b;
}

Buffer *empty_buffer(size_t size)
{

  Buffer *b = malloc(sizeof(Buffer));
  b->size = size;
  b->buffer = malloc(size + 1);
  b->buffer[size] = '\0';
  b->cursor = 0;

  return b;
}

Buffer *create_write_buffer(char *filename)
{
  Buffer *b = malloc(sizeof(Buffer));
  b->stream = fopen(filename, "w");
  b->buffer = malloc(WRITE_BUF_SIZE + 1);
  if (!b->buffer)
  {
    // ha fallado la asignacion de memoria para el fichero
    printf("malloc ha fallado para %s\n", filename);
  }
  return b;
}

char nextc(Buffer *b)
{

  char c;

  c = b->buffer[b->index];
  b->index++;
  ++b->cursor;

  return c;
}

struct PerfList
{

  clock_t get_next_clocks;
  size_t get_next_calls;

  clock_t parse_clocks;
  size_t parse_calls;

  clock_t search_clocks;
  size_t search_calls;

  clock_t elemerge_clocks;
  size_t elemerge_calls;

  clock_t addref_clocks;
  size_t addref_calls;
};

static struct PerfList f_perf;
// f_perf = malloc(sizeof(struct PerfList));

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

  int refcount;
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

Element *root_elements[ELEMENT_HASH_SIZE]; // might as well reuse EHS
Element *first_element[ELEMENT_HASH_SIZE];

// logical table source list and fun
Element *logical_table_source[LOGICAL_TABLE_SOURCE_SIZE];
unsigned long int logical_table_source_counter = 0;

unsigned long int logical_table_source_add(Element *tsource)
{

  assert(tsource);

  // check si no hemos llegado al limite del numero de table sources
  assert(logical_table_source_counter < LOGICAL_TABLE_SOURCE_SIZE);
  logical_table_source[logical_table_source_counter] = tsource;

  logical_table_source_counter++;

  return logical_table_source_counter - 1;
}

void ref_erase_all(Element *ele);

int ref_check(Element *tag)
{
  if (!tag)
  {
    printf("No tag provided to ref_check()\n");
    return 0;
  }

  if (tag->refcount == 0)
  {
    tag->removal = 1;
    ref_erase_all(tag); // ripple
    return 1;
  }

  return 0;
}

Element *ele_create(unsigned long id /*, Tag *tag*/)
{

  Element *new = malloc(sizeof(Element));

  hash_t hash = (id & ELEMENT_HASH_MASK);

  new->id = id;

  new->raw = 0;
  new->rawsize = 0;

  new->next_tag = 0;

  // new->next = 0;
  new->hash_next = 0;
  // new->last = new;
  new->hash_last = new;

  new->tagname = malloc(MAX_TAG_NAME + 1);
  *new->tagname = '\0';
  new->appname = malloc(APP_NAME_SIZE + 1); // tipicamente 6+1
  *new->appname = '\0';
  new->elename = malloc(ELEMENT_NAME_SIZE + 1);
  *new->elename = '\0';
  new->refcount = -100; // negativo hasta que se añadan refs

  new->parent = 0;
  new->child = 0;
  new->sibling = 0;

  new->physical = 0;
  new->logical = 0;

  new->removal = 0;
  new->root = 0;

  // new->ref_physical_table = 0;
  // new->logical_table_source = 0;

  new->ref = 0;
  new->ref_to = 0;

  if (id)
  {
    if (!first_element[hash])
    {
      first_element[hash] = new;

      // new->size = 0;
    }
    else
    {
      first_element[hash]->hash_last->hash_next = new;
    }

    first_element[hash]->hash_last = new;
  }

  // hm
  // if (!first_element[0])
  // {
  //   ele_create(0);
  // }

  /*
  first_element[0]->last->next = new;
  first_element[0]->last = new;
*/
  return new;
}

// searches element with 'id' using hashes
Element *ele_search(unsigned long id)
{
  if (!id)
    return NULL;

  Element *found;
  hash_t hash = (id & ELEMENT_HASH_MASK);

  if (!first_element[hash])
  {
    return NULL;
  }

  for (found = first_element[hash]; found; found = found->hash_next)
  {
    if (found->id == id)
    {
      return found;
    }
  }

  // didn't find the id in the hashed table
  return NULL;
}

// looks for element that matches id
// ->if match, returns element
// ->if no match, creates and returns element
Element *ele_merge(unsigned long id /*, Tag *tag*/)
{

  clock_t start = clock();

  Element *ele;
  ele = ele_search(id);

  if (!ele)
  {
    ele = ele_create(id);
  }

  f_perf.elemerge_calls++;
  f_perf.elemerge_clocks += clock() - start;

  return ele;
}

// añade un hijo 'child_ele' al padre 'parent_id'
Element *ele_add_child(unsigned long parent_id, Element *child_ele)
{

  Element *parent_ele = ele_merge(parent_id);
  if (!parent_ele)
  {
    printf("Error; no se ha podido encontrar ni crear el elemento %lu\n", parent_id); //no debería pasar esto
    return 0;
  }

  child_ele->parent = parent_ele; // asignar el parent

  Element *iter = parent_ele->child;

  if (!iter)
  {
    parent_ele->child = child_ele;
  }
  else
  {
    for (; iter->sibling; iter = iter->sibling)
      ;
    iter->sibling = child_ele;
  }

  return parent_ele;
}

Ref *ref_create(Element *from, Element *to)
{

  Ref *new = malloc(sizeof(Ref));

  new->start = 0;
  new->length = 0;
  new->removed = 0;

  new->from = from;
  new->to = to;

  new->next_from = 0;
  new->next_to = 0;

  return new;
}

// adds the reference chid_ref to the parent, returns the parent element
Ref *ref_add(Element *from, Element *to, char *string_start, size_t string_length)
{
  clock_t start = clock();
  // Element *parent_ele = ele_merge(parent_id);
  assert(from);
  assert(to);

  Ref *ref = ref_create(from, to);

  ref->to = to;
  ref->from = from;

  ref->start = string_start;
  ref->length = string_length;

  Ref *iter = to->ref;

  if (iter)
  {
    for (; iter->next_to; iter = iter->next_to)
      ;
    iter->next_to = ref;
  }
  else
  {
    to->ref = ref;
  }

  // si no hay ID, no hace falta asignar la ref de origen
  if (from->id)
  {
    iter = from->ref_to;

    if (iter)
    {
      for (; iter->next_from; iter = iter->next_from)
        ;
      iter->next_from = ref;
    }
    else
    {
      from->ref_to = ref;
    }
  }

  if (from->refcount < 0)
    from->refcount = 0;
  from->refcount++;

  f_perf.addref_clocks += clock() - start;
  f_perf.addref_calls++;

  return ref;
}

void ref_erase(Ref *ref)
{

  if (!ref || ref->removed)
    return;

  char *string_start = ref->start;
  size_t string_length = ref->length;
  size_t i;

  for (i = 0; i < string_length; i++)
  {
    string_start[i] = ' ';
  }

  ref->removed = 1;
  ref->from->refcount--;

  if (ref_check(ref->from))
    ;

  return;
}

// erase ALL the references to a certain [ele]
void ref_erase_all(Element *ele)
{
  if (!ele || !ele->ref)
    return;

  Ref *iref;

  for (iref = ele->ref; iref; iref = iref->next_to)
  {
    ref_erase(iref);
  }
  return;
}

int tagname_is(Element *ele, char *str)
{
  if (!ele || !str || !ele->tagname)
  {
    printf("tagname_is assert fail!\n");
    return 0;
  }
  return strncmp(ele->tagname, str, strlen(str)) == 0;
}

int tag_is_root(Element *ctag)
{
  // int bool = 0;
  switch (ctag->tagname[0])
  {
  case 'D':
  case 'B':
  case 'I':
  case 'P':
    if (tagname_is(ctag, "Database") || tagname_is(ctag, "BusinessModel") || tagname_is(ctag, "PresentationCatalog") || tagname_is(ctag, "InitBlock"))
    {
      return 1;
    }
    break;
  }

  return 0;
}

// tries to guess what gar app the string [source] is
//   and writes it to [dest]
void extract_app(char *dest, char *source)
{
  static char *transform = 0;
  // size_t i;
  int swap = 0;

  if (!transform)
  {
    transform = malloc(MAX_TAG_SIZE * sizeof(char));
  }

  strncpy(transform, source, MAX_TAG_SIZE - 1);

  // printf("<-\t\tbefore: %s\n", transform);
  skip_between(transform, "&", ";");
  // printf("<-\t\tafter_skip: %s\n", transform);
  if (transform[0] == 'R' && transform[1] == '_')
  {
    transform += 2;
    swap = 1;
  }

  str_to_alpha(transform);
  upper(transform);
  left(transform, 6);
  // printf("<-\t\tafter_left: %s\n", transform);
  strcpy(dest, transform);
  if (swap)
  {
    transform -= 2;
  }
}

// stores the next full level 1 tag in sbuf
char *get_next_tag(Buffer *source)
{
  // size_t i, j;
  static char *closing_tag = 0;

  clock_t begin = clock();

  if (!closing_tag)
    closing_tag = malloc(MAX_TAG_NAME);

  // closing_tag = "</"; // i think this is pretty bad. yikes
  strcpy(closing_tag, "</");

  char *start, *cur;

  start = strchr(source->buffer, '<');

  // printf("%.*s \n", 20, start);

  if (!start || start[1] == '/')
    return 0; //o se ha acabado o hemos chocado con un closing tag fuera de sitio

  cur = start + 1;

  // copy the element type to the closing_tag string, then add '>' (e.g. "</PresentationColumn>")
  cur = copy_until(closing_tag + 2, cur, " >");
  strcat(closing_tag, ">");

  assert(cur);

  cur = strstr(cur, closing_tag);

  assert(cur);
  cur = skip_until(cur, "\r\n");
  assert(cur);

  // insert a CUT
  *cur = '\0';

  // move the buffer position
  ++cur;
  cur = skip_all(cur, "\r\n ");
  source->buffer = cur;

  clock_t end = clock();
  f_perf.get_next_calls++;
  f_perf.get_next_clocks += (end - begin);

  return start;
}

Element *tag_parse(/*Element *ctag*/ char *raw, char **applist)
{
  if (!raw)
    return 0;

  size_t i;
  clock_t begin = clock();

  Element *ctag = 0;

  unsigned long pId = 0; // parent ID
  unsigned long id = 0;
  // Ref *new_ref = 0;

  static char *attrib = 0, *value = 0, *tagname = 0, *name = 0, *qname = 0, *pname = 0, *refid = 0;

  if (!attrib)
  {
    attrib = malloc(MAX_TAG_SIZE + 1);
    value = malloc(MAX_TAG_SIZE + 1);
    name = malloc(MAX_TAG_SIZE + 1);
    tagname = malloc(MAX_TAG_NAME + 1);
    qname = malloc(MAX_TAG_SIZE + 1);
    pname = malloc(MAX_TAG_SIZE + 1);
    refid = malloc(MAX_TAG_SIZE + 1);
  }

  *attrib = '\0';
  *value = '\0';
  *name = '\0';
  *tagname = '\0';
  *qname = '\0';
  *pname = '\0';
  *refid = '\0';

  assert(attrib);

  // char *skipped;
  char *cur = raw + 1; // b4: &ctag->raw[1] ponemos el cursor despues del '<'

  cur = copy_until_n(tagname, cur, " >", MAX_TAG_NAME);
  // printf("\ntagname: %s \n", ctag->tagname);
  assert(cur);
  cur = skip_all(cur, " \t");

  if (*cur == '>')
  { //no name, no ID
    // special cases let's go

    ctag = (ele_merge(0)); // create element with no ID
  }
  else
  {
    for (i = 0; *cur != '>'; i++)
    {

      cur = copy_until_n(attrib, cur, "=", MAX_TAG_SIZE);

      assert(cur);

      cur = skip_until(cur, "\"");
      ++cur;

      cur = copy_until_n(value, cur, "\"", MAX_TAG_SIZE);
      ++cur;
      assert(cur);

      if (strcmp(attrib, "name") == 0)
        strcpy(name, value);
      else if (strcmp(attrib, "id") == 0)
        id = encode_id(value);
      else if (strcmp(attrib, "parentId") == 0)
        pId = encode_id(value);

      cur = skip_all(cur, " \t");
    }

    if (id) // just in case..
      ctag = ele_merge(id);
  }
  assert(ctag);

  strncpy(ctag->tagname, tagname, MAX_TAG_NAME);
  strncpy(ctag->elename, name, ELEMENT_NAME_SIZE);

  char *lt = 0;
  char *gt = 0;
  Element *ref_to;
  Ref *new_ref;

  // Ref *new_ref;

  // Ref-tags loop!
  for (i = 0; *cur != '\0'; i++)
  {

    lt = strstr(cur, "<");

    if (!lt)
      break;
    cur = lt;

    if (strncmp(cur, "<![CDATA[", 9) == 0)
    {
      cur = strstr(cur, "]]>");
      assert(cur);
      continue;
    }

    if (strncmp(cur, "<!--", 4) == 0)
    {
      cur = strstr(cur, "-->");
      assert(cur);
      continue;
    }

    if (strncmp(cur, "<Ref", 4) == 0)
    {
      gt = strstr(cur, ">");
      cur = strstr(cur, " id=\"");
      // comprobar que la id sea de la Ref y no nos hemos pasado
      assert(gt > cur);
      assert(cur);
      cur = strchr(cur, '"');
      ++cur;
      cur = copy_until_n(refid, cur, "\"", MAX_TAG_SIZE);

      assert(gt > cur);

      cur = gt;

      // caso especial de las refs de privilegepackage (2 en 1)
      //
      if (strncmp(lt + 1, "RefDatabase", 11) == 0 && strncmp(ctag->tagname, "PrivilegePackage", 16) == 0)
      {
        // printf("{%.*s} ", 10,lt);
        cur++;
        cur = strchr(cur, '>');
        assert(cur);
        gt = cur;
      }

      if (strncmp(ctag->tagname, "PresentationColumn", 18) == 0)
      {
        ctag->physical = ele_search(encode_id(refid));
        // assert(ctag->element->physical);
      }

      //  adding references

      ref_to = ele_merge(encode_id(refid)); // look for the refd element
      new_ref = ref_add(ctag, ref_to, lt, (size_t)(gt - lt + 1));
      assert(new_ref);

      // new_ref = ref_create(ctag->element);
      // Element *refid_ele = ref_add(encode_id(refid), new_ref, lt, (size_t)(gt - lt + 1));

      if (tagname_is(ctag, "LogicalTableSource"))
      {
        logical_table_source_add(ctag);
      }

      /*
      if (strncmp(lt + 1, "RefPhysicalTable", 16) == 0)
      {
        ctag->ref_physical_table = ref_to;
      }*/

      assert(cur);
      continue;
    }

    cur = strstr(cur, ">");
    // ++cur;

    // cur = skip_all(cur, "> \t\n\r");
  }

  ctag->root = tag_is_root(ctag);
  if (ctag->root)
  {
    // printf("%s !\n", ctag->tagname);
    for (i = 0; root_elements[i]; i++)
      ;
    root_elements[i] = ctag;
  }

  if (ctag->root == 1 && name && *name)
  {
    // printf("><\t name is : %s\n", name);
    extract_app(ctag->appname, name);

    if (!tagname_is(ctag, "BusinessModel") && !tagname_is(ctag, "PresentationCatalog") && str_in_list(ctag->appname, applist))
    {
      ctag->removal = 1;
      // add_refid(ctag->id);
      // add to kill tag list!!
    }
  }
  else if (ctag->root == 0 && pId)
  {
    // ctag->parent = add_child(pId, ctag); CHANGE
    assert(ele_add_child(pId, ctag));
  }

  ctag->raw = raw;
  ctag->rawsize = (size_t)(cur - ctag->raw + 1);

  clock_t end = clock();
  f_perf.parse_calls++;
  f_perf.parse_clocks += (end - begin);
  return ctag;
}

void logical_table_source_parse(Element *tsource /*, unsigned long ref_ptable_id*/)
{

  assert(tsource);
  assert(tagname_is(tsource, "LogicalTableSource"));

  // unsigned long ref_ptable_id = tsource->ref_physical_table;

  // Element *ref_ptable = tsource->ref_physical_table; //ele_search(ref_ptable_id);
  Element *ref_ptable = tsource->ref_to->to;

  assert(ref_ptable);
  assert(tagname_is(ref_ptable, "PhysicalTable"));

  Element *database = ref_ptable->parent->parent;
  if (!database)
    return;
  assert(tagname_is(database, "Database"));

  assert(tsource->parent);
  assert(tsource->parent->parent);

  Element *business_model = tsource->parent->parent;
  assert(tagname_is(business_model, "BusinessModel"));
  Element *logical_table = tsource->parent;
  assert(tagname_is(logical_table, "LogicalTable"));

  assert(logical_table->child);

  Element *iter;

  for (iter = logical_table->child; iter && iter->sibling && !tagname_is(iter, "LogicalColumn") && !iter->ref; iter = iter->sibling)
    ;

  if (!iter->ref)
    return;

  assert(iter->ref);
  Ref *jter;

  for (jter = iter->ref; jter /*&& jter->next*/ && !tagname_is(jter->from, "PresentationColumn"); jter = jter->next_to)
  {
    if (!jter->next_to)
    {
      return;
    }
  };

  assert(jter->from->parent);
  assert(tagname_is(jter->from->parent, "PresentationTable"));
  assert(jter->from->parent->parent);

  Element *presentation_catalog = jter->from->parent->parent;
  assert(tagname_is(presentation_catalog, "PresentationCatalog"));

  presentation_catalog->physical = database;
  presentation_catalog->logical = business_model;
  business_model->physical = database;
  database->physical = database;
}

int html_result(t_args *arglist)
{

  // Element *iter;
  size_t i;

  char *html_header = "<!doctype html>\n\
<html lang=\"es-ES\">\n\
\n\
<head>\n\
    <meta charset=\"UTF-8\" />\n\
    <style>\n\
        body {\n\
            font-family: consolas, monospace;\n\
            font-size: 11pt;\n\
            max-width: 1000px;\n\
            margin: 2em auto;\n\
        }\n\
        table {\n\
          border-spacing: 0;\n\
        }\n\
\n\
        td,\n\
        th {\n\
            width: 280px;\n\
            border-bottom: 1px solid grey;\n\
            /*border-radius: 3px;*/\n\
            padding: 3px 5px;\n\
        }\n\
\n\
        .remove {\n\
            background-color: lightcoral;\n\
        }\n\
\n\
    </style>\n\
</head>\n\
<body>\n";
  char *body_header = "\n\
    <table>\n\
        <thead>\n\
            <tr>\n\
                <th>Presentacion</th>\n\
                <th>Logico</th>\n\
                <th>Fisico</th>\n\
            </tr>\n\
        </thead>\n";
  char *html_footer = "\n</html>";

  FILE *f = fopen("resultados.html", "wb");
  assert(f);

  fprintf(f, html_header);
  fprintf(f, "<h2>%s -> %s</h2>", arglist->input, arglist->output);
  fprintf(f, "<h3>aplicaciones eliminadas: ");
  for (i = 0; i < arglist->napps; i++)
  {
    fprintf(f, "%s ", arglist->apps[i]);
  }
  fprintf(f, "</h3>\n");

  fprintf(f, body_header);
  fprintf(f, "<tbody>\n");

  for (i = 0; root_elements[i]; i++)
  {
    if (!tagname_is(root_elements[i], "PresentationCatalog") /*|| !root_elements[i]->logical*/)
    {
      // printf("->%s - %s - %s\n", root_elements[i]->tagname, root_elements[i]->appname, root_elements[i]->elename);
      continue;
    }

    if (root_elements[i]->removal)
    {
      fprintf(f, "<tr class=\"remove\">");
    }
    else
    {
      fprintf(f, "<tr>");
    }

    // assert(root_elements[i]->logical);
    // assert(root_elements[i]->physical);

    fprintf(f, "<td>%s</td>", root_elements[i]->elename);
    if (root_elements[i]->logical)
    {
      fprintf(f, "<td>%s</td>", root_elements[i]->logical->elename);
    }
    else
    {
      fprintf(f, "<td>?</td>");
    }
    if (root_elements[i]->physical)
    {
      fprintf(f, "<td>%s</td>", root_elements[i]->physical->elename);
    }
    else
    {
      fprintf(f, "<td>?</td>");
    }

    fprintf(f, "</tr>");
  }
  fprintf(f, "</tbody>\n");

  fprintf(f, html_footer);

  fclose(f);
}

int main(int argc, char **argv)
{
  size_t i;

  /*
  char *memstring = malloc(100);
  strcpy(memstring, " first!!!.\t\t\r\n.second. \t\n\r\n   ") ;
  strip_whitespace(memstring);
  exit(0);
  */

  t_args arglist = get_args(argc, argv);

  clock_t begin = clock();

  Buffer *input_buffer = file_to_buffer(arglist.input);

  if (input_buffer == 0)
  {
    printf("Error: Ha fallado la carga del fichero XML\n");
    return EXIT_FAILURE;
  }

  // printf("Se ha copiado el fichero XML a la memoria\n");
  // Buffer *output_buffer = empty_buffer(WRITE_BUF_SIZE);
  char *rawtag = 0;

  printf("El fichero de salida es %s\n", arglist.output);
  FILE *oxml = fopen(arglist.output, "wb");
  assert(oxml);

  char *header_end = strstr(input_buffer->buffer, "<DECLARE>") + 9;
  // printf("header end set\n");

  for (i = 0; input_buffer->buffer != header_end; i++)
  {
    fputc(*input_buffer->buffer, oxml);
    //printf("char %c\n", *input_buffer->buffer);
    ++input_buffer->buffer;
  }
  fputc('\n', oxml);

  input_buffer->buffer = strstr(input_buffer->buffer, "<");

  Element *first_tag;
  Element *itag;

  printf("Empieza el parseo del XML -> ");
  // clock_t aa, zz, cparse = 0, cparsea;
  // aa = clock();
  size_t load_limit = 256;
  Element *prev_tag = 0;

  for (i = 0;; i++)
  {
    // printf("Processing tag number %i -> ", i);

    rawtag = get_next_tag(input_buffer);
    if (!rawtag) // EOF!
    {
      break;
    }

    // cparsea = clock();
    itag = tag_parse(rawtag, arglist.apps);
    // cparse += clock() - cparsea;

    if (i % load_limit == 0)
    {
      load_limit = load_limit * 2 - 1;
      // printf("\n%lu\n", load_limit);
      printf(".");
      // zz = clock();
      // printf("-> %u", f_perf.addref_clocks); //PROBLEM WITH REFS!!
      // f_perf.addref_clocks = 0;
      // printf("%u\n", cparse);
      // printf("%s\n", itag->elename);
      // aa = clock();
      // cparse = 0;
    }

    if (i == 0)
    {
      first_tag = itag;
    }
    else
    {
      prev_tag->next_tag = itag;
      // printf("assign");
    }
    prev_tag = itag;

    assert(itag);
  }
  itag->next_tag = 0;

  printf(" completado!\n(%I64u elementos)\n", i);

  /* logical table source loop */
  for (i = 0; i < logical_table_source_counter; i++)
  {
    logical_table_source_parse(logical_table_source[i]);
  }

  // printf("starting output tree.xml...\n"); // DEBUG ARBOL
  // FILE *tree_xml = fopen("tree.xml", "wb"); // DEBUG ARBOL
  // assert(tree_xml); // DEBUG ARBOL

  int depth;

  Element *iter;
  Element *jter;
  Ref *kter;

  unsigned long int count = 0;
  for (iter = first_tag /*first_element[0]*/; iter; iter = iter->next_tag)
  {
    // printf("%lu\n", iter->id);
    count++;
  }
  // printf("afterloop\n");
  // printf("count: %lu elements", count);

  printf("\n-----\n");
  for (i = 0; root_elements[i]; i++)
  {
    iter = root_elements[i];
    // printf("\n->%s [%s]", iter->tag->tagname, iter->tag->appname);
    depth = 0;

    if (tagname_is(iter, "PresentationCatalog"))
    {
      if (iter && iter->physical)
      {
        // printf("SA '%s' -> DB '%s'\n", iter->tag->appname, iter->physical->tag->appname);
      }
    }

    if (iter->physical && iter->physical->removal == 1)
    {
      // printf("*Se marca %s (%s) a causa de %s (%s)\n", iter->appname, iter->tagname, iter->physical->appname, iter->physical->tagname);
      iter->removal = 1;
    }

    if (iter->removal == 1)
    {

      printf("->%s [%s]", iter->tagname, iter->elename);
      printf("<-(eliminado)!\n");

      if (tagname_is(iter, "ConnectionPool"))
      {
        for (kter = iter->ref; kter; kter = kter->next_to)
        {
          if (tagname_is(kter->from, "InitBlock"))
          {
            kter->from->removal = 1;
          }
        }
      }

      for (jter = iter; 1;)
      {
        // printf("%s has sibling %x and parent %x\n", jtag->tagname, jtag->sibling, jtag->parent);

        // fprintf(tree_xml, "%.*s%s (id=%lu)\n", depth, "\t\t\t\t\t", jter->tagname, jter->id); // DEBUG ARBOL
        jter->removal = 1;

        ref_erase_all(jter);

        if (jter->child)
        {
          jter = jter->child;
          depth++;
        }
        else if (jter->sibling)
        {
          jter = jter->sibling;
        }
        else
        {
          while (!jter->sibling && jter->parent)
          {
            jter = jter->parent;
            // fprintf(tree_xml, "\n"); // DEBUG ARBOL
            depth--;
          }
          if (!jter->sibling && !jter->parent)
            break;
          jter = jter->sibling;
        }
      }
    }
  }

  // fclose(tree_xml); // DEBUG ARBOL

  Element *tag_iter;

  for (tag_iter = first_tag; tag_iter; tag_iter = tag_iter->next_tag)
  {
    // printf("->%lu", tag_iter->id);
    if (!tag_iter->removal)
    {
      fwrite(tag_iter->raw, 1, tag_iter->rawsize, oxml);
      // printf("%s\n", tag_iter->raw);
      fwrite("\n", 1, 1, oxml);
    }
  }

  input_buffer->buffer = strstr(input_buffer->buffer, "<");
  for (i = 0; *input_buffer->buffer; i++)
  {
    //printf("->(%c)[%u]\n", *input_buffer->buffer, *input_buffer->buffer);
    fputc(*input_buffer->buffer, oxml);
    ++input_buffer->buffer;
  }

  fclose(oxml);

  // clock_t end = clock();

  // printf("Total:\t%u clocks\n", end - begin);
  // printf("Next tag:\t%u clocks\t%u calls\n", f_perf.get_next_clocks, f_perf.get_next_calls);
  // printf("Parsing:\t%u clocks\t%u calls\n", f_perf.parse_clocks, f_perf.parse_calls);
  // printf("Search:\t%u clocks\t%u calls\n", f_perf.search_clocks, f_perf.search_calls);

  clock_t end = clock();

  html_result(&arglist);

  printf("\n-----\nHa finalizado la ejecucion (correctamente) tras %f segundos.\n", (double)(end - begin) / CLOCKS_PER_SEC);

  // printf("sizeof Tag: \nsizeof Ele: %I64u\nsizeof Ref: %I64u\n", sizeof(Element), sizeof(Ref));

  exit(EXIT_SUCCESS);
}
