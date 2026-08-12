#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"

#define MAX_ROWS 1024

struct Row {
  int key;
  char value[256];
};

extern struct Row data[MAX_ROWS];
extern int data_capacity;

void parse(const struct statement* st);
void load_data();

#endif
