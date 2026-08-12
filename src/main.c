#include <stdio.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/tokenizer.h"

void prompt();

int main() {
 
  const int buffer_size = 256;
  char buffer[buffer_size];

  load_data();
  prompt();

  while(fgets(buffer, buffer_size, stdin) != NULL) {

    size_t idx = strcspn(buffer, "\n");
    buffer[idx] = '\0';

    if(strcmp(buffer, "QUIT") == 0) {
      return 0;
    }

    struct statement st = tokenize(buffer);
//    print_statement(&st);
    parse(&st);
    prompt();
  }

  return 0;
}

void prompt() {
  printf(">");
  fflush(stdout);
}
