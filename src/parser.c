#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/parser.h"
#include "../include/tokenizer.h"
#include "../include/hash_map.h"
#include "../include/util.h"

struct hash_map* map = NULL;
FILE* file = NULL;
bool is_loading = false;

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

  if(!is_loading) {
    fprintf(file, "SET %s %s\n", key, value);
    fflush(file);
  }

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

  if(!is_loading) {
    fprintf(file, "DELETE %s\n", key);
    fflush(file);
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
  if(!file) {
    printf("[ERR] Failed to save the data!\n");
    return;
  }

  hash_map_iterate(map, append_data_callback, file);
}

static void handle_quit_command() {
  exit(0);
}

static void handle_compact_command() {
  if(!file) {
    printf("[ERR] Failed to compact data!\n");
    return;
  }

  ftruncate(fileno(file), 0);
  rewind(file);
  hash_map_iterate(map, append_data_callback, file);
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
  else if(strcmp(operation, "QUIT") == 0) {
    handle_quit_command();
  }
  else if(strcmp(operation, "SAVE") == 0) {
    handle_save_command();
  }
  else if(strcmp(operation, "COMPACT") == 0) {
    handle_compact_command();
  }
  else {
    printf("[ERR] Operation '%s' does not exist!\n", operation);
  }
}

void load_data() {
  map = hash_map_init(16, 0.75);
  if(!map) {
    printf("[ERR] Could not load the data. Failed to initialize the hash map\n");
    return;
  }

  file = fopen("data.txt", "a+");
  if(!file) {
    printf("[ERR] Could not load the data. Failed to open the file\n");
    return;
  }
  rewind(file); // Start reading from beginning
  
  is_loading = true;

  char buffer[256];
  while(fgets(buffer, 256, file)) {
    trim_newline(buffer);
    struct statement st = tokenize(buffer);
    parse(&st);
  }

  is_loading = false;
}
