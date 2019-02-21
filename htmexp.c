// #pragma once

#include <stdio.h>
#include "args.h"
#include "htmexp.h"
#include "xp.h"
#include "strmsc.h"

void html_result(struct t_args arglist, struct Element **root_elements)
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

  fprintf(f, "%s", html_header);
  fprintf(f, "<h2>%s -> %s</h2>", arglist.input, arglist.output);
  fprintf(f, "<h3>aplicaciones eliminadas: ");
  for (i = 0; i < arglist.napps; i++)
  {
    fprintf(f, "%s ", arglist.apps[i]);
  }
  fprintf(f, "</h3>\n");

  fprintf(f, "%s" , body_header);
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

  fprintf(f, "%s", html_footer);

  fclose(f);

  return;
}