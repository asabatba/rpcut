

struct t_args
{
  char *input;
  char *output;
  char **apps;
  int napps;
} ;

struct t_args get_args(int argc, char **argv);