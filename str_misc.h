
#define copy_until(dest, source, tok) copy_until_n(dest, source, tok, 0)

void str_to_alpha(char *input);

void left(char *input, int newlength);

void upper(char *input);

size_t char_count(char *string);

size_t str_replace(char *string, char a, char b);

int str_in_list(char *app, char **list);

unsigned long power2(unsigned long exp);

unsigned long encode_id(char *strid);

char is_any_of(char c, char *alts);

char *copy_until_n(char *dest, char *source, char *tokens, size_t n);

char *skip_until(char *source, char *tokens);

char *skip_all(char *source, char *tokens);

char *ignore_between(char *string, char *start, char *end);

char *strip_whitespace(char *string);

char *skip_between(char *source, char *start, char *end);
