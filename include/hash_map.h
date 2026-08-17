#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_KEY_LENGTH 256
#define MAX_VALUE_LENGTH 256

struct node {
  char key[MAX_KEY_LENGTH];
  char value[MAX_VALUE_LENGTH];
  struct node* next;
};

struct hash_map {
  struct node** bucket_array;
  size_t capacity;
  size_t current_size;
};

typedef void(*hash_map_callback)(const char* key, const char* value, void* context);

struct hash_map* hash_map_init(size_t size);
void hash_map_free(struct hash_map* map);
void hash_map_put(struct hash_map* map, const char* key, const char* value);
const char* hash_map_get(const struct hash_map* map, const char* key);
bool hash_map_delete(struct hash_map* map, const char* key);
void hash_map_iterate(const struct hash_map* map, hash_map_callback callback, void* context);

#endif
