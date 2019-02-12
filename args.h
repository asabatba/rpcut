

typedef struct t_args
{
  char *input;
  char *output;
  char **apps;
  int napps;
} t_args;

t_args get_args(int argc, char **argv);