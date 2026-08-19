#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/ui_log.h"
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
    UI_ERROR("Invalid or missing key!");
    return;
  }

  const char* value = hash_map_get(map, key);
  if(value) {
    printf("%s\n", value);
    fflush(stdout);
  }
  else {
    UI_ERROR("Entry for key '%s' does not exist", key);
  }
}

static void handle_set_command(const struct statement* st) {
  if(st->token_count < 3) {
    UI_ERROR("'SET' command needs 3 arguments!");
    return;
  }

  const char* key;
  if(!get_key(st, &key)) {
    UI_ERROR("Invalid or missing key!");
    return;
  }

  const char* value = st->tokens[2].value;
  hash_map_put(map, key, value);

  if(!is_loading) {
    fprintf(file, "SET %s %s\n", key, value);
    fflush(file);
    UI_SUCCESS("Successfully set '%s' to '%s'.", key, value);
  }
}

static void handle_delete_command(const struct statement* st) {
  const char* key;
  if(!get_key(st, &key)) {
    UI_ERROR("Invalid or missing key!");
    return;
  }

  if(hash_map_delete(map, key)) {
    if(!is_loading) {
      UI_SUCCESS("Deleted entry for key '%s'", key);
      fprintf(file, "DELETE %s\n", key);
      fflush(file);
    }
  }
  else {
    if(!is_loading) {
      UI_ERROR("Could not delete entry for key '%s'", key);
    }
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
    UI_ERROR("Failed to run the command!");
    return;
  }

  hash_map_iterate(map, append_data_callback, file);
}

static void handle_quit_command() {
  exit(0);
}

static void handle_compact_command() {
  if(!file) {
    UI_ERROR("Failed to run the command!");
    return;
  }

  ftruncate(fileno(file), 0);
  rewind(file);
  hash_map_iterate(map, append_data_callback, file);
}

void parse(const struct statement* st) {
  if(!st) {
    UI_ERROR("Failed to parse the command!");
    return;
  }

  if(st->token_count == 0) {
    UI_ERROR("Cannot parse empty statement!");
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
    UI_ERROR("Operation '%s' does not exist!", operation);
  }
}

void load_data() {
  map = hash_map_init(16, 0.75);
  if(!map) {
    UI_ERROR("Could not start application (System out of memory).");
    return;
  }

  file = fopen("data.txt", "a+");
  if(!file) {
    UI_ERROR("Could not start application (Failed to load data).");
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
