
#include <stdlib.h>
#include <stdio.h>
// #include <time.h>
#include <string.h>

#include "args.h"
#include "str_misc.h"

typedef struct t_args t_args;

t_args get_args(int argc, char **argv)
{
  t_args output = {0};
  output.apps = calloc(argc, sizeof(char *)); //argc includes all args+1

  unsigned i, j = 0;

  for (i = 1; i < argc; i++)
  {
    // caso para el XML de entrada
    if (strcmp(argv[i], "-i") == 0 && output.input == NULL)
    {
      // comprueba que el siguiente argumento no sea un indicador -
      if (argv[i + 1] && strncmp(argv[i + 1], "-", 1))
      {
        output.input = argv[i + 1];
        i++;
      }
    }
    // caso para el XML de salida
    else if (strcmp(argv[i], "-o") == 0 && output.output == NULL)
    {
      if (argv[i + 1] && strncmp(argv[i + 1], "-", 1))
      {
        output.output = argv[i + 1];
        i++;
      }
    }
    // caso para nombres de aplicaciones
    else if (strcmp(argv[i], "-a") == 0)
    {
      for (; j < argc; j++)
      {
        i++;
        // si el string siguiente no empieza por "-", es una aplicacion
        if (argv[i] && strncmp(argv[i], "-", 1))
        {
          output.apps[j] = argv[i];
        }
        else // en caso contrario, no es aplicacion y continuar con parseo
        {
          break;
        }

        // siglas en mayusculas
        upper(argv[i]);
      }
      i--;
      // actualizar contador de aplicaciones
      output.napps = j;
    }
  }

  if (output.input == NULL)
  {
    printf("Error: Falta indicar el XML de entrada (-i)\n");
    exit(EXIT_FAILURE);
  }
  else if (output.output == NULL)
  {
    printf("Error: Falta indicar el XML de salida (-o)\n");
    exit(EXIT_FAILURE);
  }
  else if (output.napps == 0)
  {
    printf("Error: Falta indicar por lo menos el nombre de una aplicación (-a)\n");
    exit(EXIT_FAILURE);
  }
  return output;
}