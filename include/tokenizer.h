#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

#define MAX_TOKEN_LENGTH 256

struct token {
  char value[MAX_TOKEN_LENGTH];
  size_t length;
};

struct statement {
  struct token tokens[32];
  size_t token_count;
};

struct statement tokenize(char* buffer);
void print_statement(const struct statement* st);

#endif
