#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

struct token {
  char value[256];
  size_t length;
};

struct statement {
  struct token tokens[32];
  size_t token_count;
};

struct statement tokenize(char* buffer);
void print_statement(const struct statement* st);

#endif
