#include <time.h>

enum function_name
{
    GET_NEXT_TAG,
    TAG_PARSE,
    ELE_MERGE,
    REF_ADD,
};

struct Funspeed;

// suma el tiempo de procesado de la funcion
void funspeed_update(enum function_name function, clock_t start, int debug);

// escribe en la pantalla los tiempos
void funspeed_print_dbg(int debug);
