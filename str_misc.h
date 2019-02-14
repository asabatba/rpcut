
#define copy_until(dest, source, tok) copy_until_n(dest, source, tok, 0)

size_t str_to_alpha(char *input);

size_t left(char *input, int newlength);

// strncpy que termina el string dest con un caracter nulo
char * strscpy(char *dest, const char *src, size_t len);

void upper(char *input);

// size_t char_count(char *string);

size_t str_replace(char *string, char a, char b);

int str_in_list(char *app, char **list);

unsigned long power2(unsigned long exp);

char is_any_of(char c, const char *tokens);

char *copy_until_n(char *dest, char *source, const char *tokens, size_t n);

char *skip_until(char *source, const char *tokens);

char *skip_all(char *source, const char *tokens);

char *ignore_between(char *string, const char *start, const char *end);

char *strip_whitespace(char *string);

char *skip_between(char *source, const char *start, const char *end);
