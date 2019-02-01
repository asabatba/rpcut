
#include "args.h"
#include <stdlib.h>



t_args get_args(int argc, char **argv)
{
  t_args output = {0};
  output.apps = malloc(argc * sizeof(char *)); //argc includes all args+1
  // assert(output);

  int i = 1, j = 0;
  while (i < argc)
  {
    //puts(argv[i]);
    if (strcmp(argv[i], "-i") == 0 && output.input == NULL)
    {
      if (argv[i + 1] && strncmp(argv[i + 1], "-", 1))
      {
        //output.input = !output.input ? argv[i+1] : output.input;
        output.input = argv[i + 1];
        i++;
      }
      else
      {
        output.input = NULL;
      }
      //puts(output.input);
    }
    else if (strcmp(argv[i], "-o") == 0 && output.output == NULL)
    {
      if (argv[i + 1] && strncmp(argv[i + 1], "-", 1))
      {
        //output.output = !output.output ? argv[i+1] : output.output;
        output.output = argv[i + 1];
        i++;
      }
      else
      {
        output.output = NULL;
      }
    }
    else if (strcmp(argv[i], "-a") == 0)
    {
      for (; j < argc; j++)
      {
        i++;
        output.apps[j] = (argv[i] && strncmp(argv[i], "-", 1)) ? argv[i] : NULL;
        if (!output.apps[j])
          break;
        upper(argv[i]);
      }
      i--;
      output.napps = j;
    }
    i++;
  }
  // printf("%i\n", output.napps);
  if (!output.input || !output.output || !output.napps)
  {
    printf("Falta uno o mas parametros necesarios (-i, -o, -a)\n");
    exit(EXIT_FAILURE);
  }
  return output;
}