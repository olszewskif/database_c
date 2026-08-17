#include <string.h>
#include <stdio.h>
#include "../include/parser.h"
#include "../include/tokenizer.h"
#include "../include/hash_map.h"
#include "../include/util.h"

struct hash_map* map = NULL;

static bool get_key(const struct statement* st, const char** key) {
  if(st->token_count < 2 || key == NULL) {
    return false;
  }

  *key = st->tokens[1].value;
  return true;
}

static void handle_get_command(const struct statement* st) {
  const char* key;
  if(!get_key(st, &key)) {
    printf("[ERR] Invalid or missing key!\n");
    return;
  }

  const char* value = hash_map_get(map, key);
  if(value) {
    printf("%s\n", value);
  }
  else {
    printf("[ERR] Entry for key '%s' does not exist!\n", key);
  }
}

static void handle_set_command(const struct statement* st) {
  if(st->token_count < 3) {
    printf("[ERR] The 'SET' commands needs 3 arguments!\n");
    return;
  }

  const char* key;
  if(!get_key(st, &key)) {
    printf("[ERR] Invalid or missing key!\n");
    return;
  }

  const char* value = st->tokens[2].value;
  hash_map_put(map, key, value); 

  printf("Successfully set '%s' to '%s'.\n", key, value);
}

static void handle_delete_command(const struct statement* st) {
  const char* key;
  if(!get_key(st, &key)) {
    printf("[ERR] Invalid or missing key!\n");
    return;
  }

  if(hash_map_delete(map, key)) {
    printf("Deleted row for key '%s'.\n", key);
  }
  else {
    printf("[ERR] Could not delete entry for key '%s'\n", key);
  }
}

static void append_data_callback(
  const char* key, 
  const char* value,
  void* context
) {
  FILE* file = (FILE*) context;
  fprintf(file, "SET %s %s\n", key, value);
}

static void handle_save_command() {
  FILE* file = fopen("data.txt", "w");
  if(!file) {
    printf("[ERR] Failed to save the data!\n");
    return;
  }

  hash_map_iterate(map, append_data_callback, file);
  fclose(file);
}

void parse(const struct statement* st) {
  if(!st) {
    printf("Statement is null!\n");
    return;
  }

  if(st->token_count == 0) {
    printf("[ERR] Cannot parse empty statement!\n");
    return;
  }

  const char* operation = st->tokens[0].value;

  if(strcmp(operation, "GET") == 0) {
    handle_get_command(st);
  }
  else if(strcmp(operation, "SET") == 0) {
    handle_set_command(st); 
  }
  else if(strcmp(operation, "DELETE") == 0) {
    handle_delete_command(st);
  }
  else if(strcmp(operation, "SAVE") == 0) {
    handle_save_command();
  }
  else {
    printf("[ERR] Operation '%s' does not exist!\n", operation);
  }
}

void load_data() {
  map = hash_map_init(16);
  if(!map) {
    printf("[ERR] Could not load the data. Failed to initialize the hash map\n");
    return;
  }

  FILE* file = fopen("data.txt", "r");
  if(!file) {
    return;
  }

  char buffer[256];

  while(fgets(buffer, 256, file)) {
    trim_newline(buffer);
    struct statement st = tokenize(buffer);
    parse(&st);
  }

  fclose(file);
}
