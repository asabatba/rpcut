#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "str_misc.h"

// leaves only alphabet (a-z and A-Z)
void str_to_alpha(char *input)
{
  size_t i, j; //input is malloc'd
  j = 0;
  for (i = 0; input[i] != '\0'; ++i)
  {
    if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z'))
    {
      input[j] = input[i];
      j++;
    }
  }
  input[j] = '\0';
}

void left(char *input, int newlength)
{
  size_t i;
  for (i = 0; input[i] && i < newlength; i++)
    ;
  input[i] = '\0';
}

void upper(char *input)
{
  size_t i;
  for (i = 0; input[i]; ++i)
  {
    input[i] = (input[i] >= 'a' && input[i] <= 'z') ? (input[i] - 'a' + 'A') : input[i];
  }
}

// size_t char_count(char *string)
// {

//   if (!string)
//     return 0;

//   size_t i;

//   for (i = 0; string[i]; i++)
//     ;
//   return i;
// }

size_t str_replace(char *string, char a, char b)
{
  size_t i;
  for (i = 0; string[i]; i++)
  {
    if (string[i] == a)
      string[i] = b;
  }
  return i;
}

// returns 1 if app is in the list
int str_in_list(char *app, char **list)
{
  size_t i;

  // printf("\tcheck if %s in list...", app);

  for (i = 0; list[i]; i++)
  {

    //printf("%s against-->%s\n", app, list[i]);
    if (strstr(app, list[i]))
    {
      // printf("yes (%s)!\n", list[i]);
      return 1;
    }
  }
  // printf("no !\n");
  return 0;
}

unsigned long power2(unsigned long exp)
{
  size_t i;
  unsigned long output = 1;
  for (i = 0; i < exp; i++)
  {
    output = output * 2;
  }
  return output;
}

// returns a character if said character c is equal to any of the characters in string 'tokens' (NULL if not)
char is_any_of(char c, const char *tokens)
{
  size_t i;

  // assert(tokens);
  // assert(tokens[0]);
  if (!c)
    return 0;

  for (i = 0; tokens[i]; i++)
  {

    if (c == tokens[i])
    {
      //printf("{%c}", c);
      return c;
    }
    //printf("(%c)", c);
    //printf("%c", '\t');
  }
  return 0;
}

// copia todos los caracteres desde source hasta dest,
//  hasta chocar contra algun token (o NULL char)
//   (o hasta escribir n caracteres en dest)
char *copy_until_n(char *dest, char *source, const char *tokens, size_t n)
{

  size_t i;
  assert(*tokens && "this function was called with NULL arg tokens");
  assert(*source && "this function was called with NULL arg source");
  //printf("\n\n- copy -\n\n");
  if (dest)
  {
    for (i = 0; *source && !is_any_of(*source, tokens); i++)
    {
      if (n && i == n)
        break;
      dest[i] = *source;
      ++source;
    }
    dest[i] = '\0';
  }
  else
  { // just advance the source if there's no dest
    for (i = 0; *source && !is_any_of(*source, tokens); i++)
    {
      if (n && i == n)
        break;
      ++source;
    }
  }

  return source;
}

// just a wrapper
char *skip_until(char *source, const char *tokens)
{
  return copy_until(0, source, tokens);
}

// skip source as long as it's just tokens
char *skip_all(char *source, const char *tokens)
{
  //printf("\n\n- skipall -\n\n");
  assert(*tokens);
  assert(*source);
  size_t i;

  for (i = 0; *source && is_any_of(*source, tokens); i++)
    ++source;
  //return i : source ? 0;
  return source;
}

// ignore eveything between [start] and [end] strings

char *ignore_between(char *string, const char *start, const char *end)
{
  int startlen = strlen(start);

  if (strncmp(string, start, startlen) == 0)
  {
    string = strstr(string, end);
    if (!string)
      return 0;

    string = string + strlen(end);
    return string;
  }

  return 0;
}

// usage:  string = strip_whitespace(string);
char *strip_whitespace(char *string)
{
  size_t i, j, k;
  size_t length = strlen(string);

  assert(string);

  char *new_string = malloc((length + 1));

  assert(new_string);

  // strip right
  for (k = length - 1; k >= 0 && string[k] && is_any_of(string[k], "\t\r\n "); k--)
    ;

  // strip left
  for (i = 0; string[i] && is_any_of(string[i], "\t\r\n "); i++)
    ;

  // cpy

  for (j = 0; i <= k; i++)
  {
    new_string[j] = string[i];
    j++;
  }
  // printf("\"%s\"\r\n->\r\n\"%s\"", string, new_string);
  free(string);
  assert(new_string);
  return new_string;
}

// permanently modifies the string [source] so that everything
//   between [start] and [end] is ignored
char *skip_between(char *source, const char *start, const char *end)
{

  size_t i, j = 0;
  int startlen = strlen(start);
  int endlen = strlen(end);
  assert(startlen);
  assert(endlen);

  for (i = 0; source[i]; i++)
  {
    // printf("-< start %.*s vs %s\n", startlen,source + i, start) ;
    if (strncmp(source + i, start, startlen) == 0)
    {

      for (; (source + i) && strncmp(source + i, end, endlen) != 0; i++)
        ;
      i = i + endlen;
    }
    if (!source[i])
      break;
    source[j] = source[i];
    // printf("%.*s - - > %.*s\n", j,source, i, source);
    j++;
  }
  source[j] = '\0';

  return source;
}