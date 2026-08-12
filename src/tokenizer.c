#include "../include/tokenizer.h"
#include <string.h>
#include <stdio.h>

struct statement tokenize(char* buffer) {
  const char* delim = " ";
  char* token = strtok(buffer, delim);
  
  struct statement st = { 0 };
  int i = 0;

  while(token != NULL) {
    size_t length = strlen(token);
    struct token tok = { 0 };

    tok.length = length;
    strncpy(tok.value, token, length);

    st.tokens[i] = tok;

    token = strtok(NULL, delim);
    i++;
  }

  st.token_count = i;
  return st;
}

void print_statement(const struct statement* st) {
  for(size_t i = 0; i < st->token_count; ++i) {
    printf("%s ", st->tokens[i].value);
  }
  printf("\n");
}

static void handle_string(char* token) {

}
