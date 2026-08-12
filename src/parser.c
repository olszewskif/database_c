#include <string.h>
#include <stdio.h>
#include "../include/parser.h"

struct Row data[MAX_ROWS];
int data_capacity = 0;

static bool get_key(const struct statement* st, int* key) {
  if(st->token_count < 2) {
    return false;
  }

  sscanf(st->tokens[1].value, "%d", key);
  return true;
}

static bool get_index_for_key(int key, int* index) {
  for(size_t i = 0; i < data_capacity; ++i) {
    if(data[i].key == key) {
      *index = i;
      return true;
    }
  }
  return false;
}

static void handle_get_command(const struct statement* st) {
  int key;
  if(!get_key(st, &key)) {
    printf("[ERR] Invalid or missing key!\n");
    return;
  }

  int index;
  if(!get_index_for_key(key, &index)) {
    printf("[ERR] Row for key %d does not exist!\n", key);
    return;
  }

  printf("%s\n", data[index].value);
}

static void handle_set_command(const struct statement* st) {
  if(data_capacity >= MAX_ROWS) {
    printf("[ERR] Data buffer is full. Cannot set any additional row!\n");
    return;
  }

  if(st->token_count < 3) {
    printf("[ERR] The 'SET' commands needs 3 arguments!\n");
    return;
  }

  int key;
  if(!get_key(st, &key)) {
    printf("[ERR] Invalid or missing key!\n");
    return;
  }
  
  const char* value = st->tokens[2].value;
  int index;
  // Row does not exist
  if(!get_index_for_key(key, &index)) {
    struct Row row = { 0 };
    row.key = key;
    strncpy(row.value, value, 256);
    data[data_capacity++] = row;
  }
  // Row exists (overwrite)
  else {
    strncpy(data[index].value, value, 256);
  }

  printf("Successfully set '%d' to '%s'.\n", key, value);
}

static void handle_delete_command(const struct statement* st) {
  int key;
  if(!get_key(st, &key)) {
    printf("[ERR] Invalid or missing key!\n");
    return;
  }

  int index;
  if(!get_index_for_key(key, &index)) {
    printf("[ERR] Row for key %d does not exist!\n", key);
    return;
  }

  for(size_t i = index; i < data_capacity - 1; ++i) {
    data[i] = data[i + 1];
  }
  data_capacity--;
  printf("Deleted row for key '%d'.\n", key);
}

static void handle_save_command(const struct statement* st) {
  FILE* file = fopen("data.bin", "wb");
  if(!file) {
    printf("[ERR] Failed to save the data!\n");
    return;
  }

  size_t size = fwrite(data, sizeof(struct Row), data_capacity, file);
  if(size == data_capacity) {
    printf("Successfully saved the data.\n");
  }
  else {
    printf("[ERR] Error writing to file!\n");
  }

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
    handle_save_command(st);
  }
  else {
    printf("[ERR] Operation '%s' does not exist!\n", operation);
  }
}

void load_data() {
  FILE* file = fopen("data.bin", "rb");
  if(!file) {
    return;
  }

  data_capacity = fread(data, sizeof(struct Row), MAX_ROWS, file);
  fclose(file);
}
