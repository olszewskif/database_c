#include "../include/hash_map.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct node* create_node(const char* key, const char* value) {
  struct node* new_node = (struct node*) malloc(sizeof(struct node));
  strncpy(new_node->key, key, MAX_KEY_LENGTH);
  strncpy(new_node->value, value, MAX_VALUE_LENGTH);
  new_node->next = NULL;
  return new_node;
}

static void push_node(struct node** head, const char* key, 
                      const char* value) {
  if(!(*head)) {
    (*head) = create_node(key, value);
    return;
  }

  struct node* temp = *head;
  while(temp->next != NULL) {
    if(strcmp(temp->key, key) == 0) {
      strncpy(temp->value, value, MAX_VALUE_LENGTH);
      return;
    }

    temp = temp->next;
  }

  if(strcmp(temp->key, key) == 0) {
      strncpy(temp->value, value, MAX_VALUE_LENGTH);
      return;
  }

  temp->next = create_node(key, value);
}

static struct node* get_node(struct node* head, const char* key) {
  while(head != NULL) {
    if(strcmp(head->key, key) == 0) {
      return head;
    }
    head = head->next;
  }
  return NULL;
}

static bool delete_node(struct node** head, const char* key) {
  if(*head == NULL) {
    return false;
  }

  if(strcmp((*head)->key, key) == 0) {
    struct node* to_free = *head;
    *head = (*head)->next;
    free(to_free);
    return true;
  }

  struct node* temp = *head;

  while(temp->next != NULL) {
    if(strcmp(temp->next->key, key) == 0) {
      struct node* to_free = temp->next;
      temp->next = temp->next->next;
      free(to_free);
      return true;
    }
    temp = temp->next;
  }

  return false;
}

static void free_nodes(struct node* head) {
  struct node* temp = head;
  while(temp != NULL) {
    temp = head->next;
    free(head);
    head = temp;
  }
}

static void print_linked_list(struct node* head) {
  printf("Printing linked list with head key: %s\n", head->key);
  while(head != NULL) {
    printf("Key: %s, Value: %s\n", head->key, head->value);
    head = head->next;
  }
}

static int get_hash(const char* key) {
  int hash = 5381;
  for(const char* c = key; *c != '\0'; c++) {
    hash *= 31;
    hash += (int)(*c);
  }
  return hash;
}

struct hash_map* hash_map_init(size_t size) {
  struct hash_map* map = (struct hash_map*) malloc(sizeof(struct hash_map));
  map->bucket_array = (struct node**) malloc(size * sizeof(struct node*));
  for(size_t i = 0; i < size; ++i) {
    map->bucket_array[i] = NULL;
  }
  map->capacity = size;
  map->current_size = 0;
  return map;
}

void hash_map_free(struct hash_map* map) {
  for(size_t i = 0; i < map->capacity; ++i) {
    free_nodes(map->bucket_array[i]);
  }
  free(map->bucket_array);
  free(map);
}

void hash_map_put(struct hash_map* map, const char* key, 
                  const char* value) {
  int hash = get_hash(key) % map->capacity;
  struct node* head = map->bucket_array[hash];
  push_node(&head, key, value);

  if(map->bucket_array[hash] == NULL) {
    map->bucket_array[hash] = head;
  }
  print_linked_list(head);
}

const char* hash_map_get(const struct hash_map* map, const char* key) {
  int hash = get_hash(key) % map->capacity;
  struct node* head = map->bucket_array[hash];
  struct node* found = get_node(head, key);
  if(!found) {
    printf("[ERR] Failed to find node with key %s!\n", key);
    return NULL;
  }
  return found->value;
}

bool hash_map_delete(struct hash_map* map, const char* key) {
  int hash = get_hash(key) % map->capacity;
  struct node** head = &map->bucket_array[hash];
  bool res = delete_node(head, key);
  if(res) {
    printf("Successfully deleted entry with key '%s'\n", key);
  }
  else {
    printf("Could not find an entry with key '%s'\n", key);
  }
  return res;
}

void hash_map_iterate(
  const struct hash_map* map,
  hash_map_callback callback,
  void* context
) {
  for(size_t i = 0; i < map->capacity; ++i) {
    struct node* head = map->bucket_array[i];
    while(head != NULL) {
      callback(head->key, head->value, context);
      head = head->next;
    }
  }
}
