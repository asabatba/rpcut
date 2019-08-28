// #pragma once

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#include "xpfun.h"
#include "args.h"
#include "strmsc.h"

int main(int argc, char **argv)
{
  size_t i;

  printf("sizeof Element: %I64u\n", sizeof(Element));

  // funcion que carga los argumentos en el struct arglist
  struct t_args arglist = get_args(argc, argv);

  clock_t begin = clock();

  // carga del fichero input XML
  struct Buffer *input_buffer = file_to_buffer(arglist.input);

  if (input_buffer == NULL)
  {
    printf("Error: Ha fallado la carga del fichero XML\n");
    return EXIT_FAILURE;
  }

  // open del fichero output XML
  printf("El fichero de salida es %s\n", arglist.output);
  FILE *oxml = fopen(arglist.output, "wb");
  if (oxml == NULL)
  {
    printf("Error: No se ha podido abrir el fichero de salida %s\n", arglist.output);
    exit(EXIT_FAILURE);
  }

  // copia el inicio del XML de entrada a la salida
  if (!parse_header(input_buffer, oxml))
    exit(EXIT_FAILURE);

  // Iteracion de parseo del XML
  printf("Empieza el parseo del XML -> ");

  size_t load_limit = 1024;
  Element *first_tag = 0;
  Element *itag = 0;
  Element *prev_tag = 0;
  char *rawtag = 0; // pointer al inicio de la etiqueta que esta en memoria

  for (i = 0;; i++)
  {
    // printf("Processing tag number %i -> ", i);

    rawtag = get_next_tag(input_buffer);
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

  itag->next_tag = 0;

  printf(" completado!\n(%lu elementos)\n", (long unsigned)i);

  // loop que busca relaciones entre elementos de distintas capas
  logical_table_source_loop();

  printf("\n--***--\n");

  // decide que elementos se quedan y cuales se quitan
  ele_decider();

  // guarda el output teniendo en cuenta los elemntos que se quitan
  save_xml(oxml, first_tag, input_buffer);
  fclose(oxml);

  clock_t end = clock();

  printf("\n--***--\n");
  // analisis de velocidad de algunas funciones
  funspeed_print();

  // export a html de los resultados
  printf("Se exportan los resultados a html.\n");
  html_result(arglist);

  printf("\n--***--\n");
  printf("Ha finalizado la ejecucion (correctamente) tras %f segundos.\n", (double)(end - begin) / CLOCKS_PER_SEC);

  exit(EXIT_SUCCESS);
}
