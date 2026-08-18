#include "../include/hash_map.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct node* create_node(const char* key, const char* value) {
  struct node* new_node = (struct node*) malloc(sizeof(struct node));
  if(new_node == NULL) {
    return NULL;
  }

  strncpy(new_node->key, key, MAX_KEY_LENGTH);
  strncpy(new_node->value, value, MAX_VALUE_LENGTH);
  new_node->next = NULL;
  return new_node;
}

/*
  Pushes node into linked list with specified head.
  If the key exists and the operation is replacement, return 0 (0 new nodes)
  If new node has been created and inserted, return 1 (1 new node)
  If an error occurs, return -1
*/
static int push_node(struct node** head, const char* key, 
                      const char* value) {
  if(*head == NULL) {
    *head = create_node(key, value);
    if(*head == NULL) {
      return -1;
    }

    return 1;
  }

  struct node* temp = *head;
  for(;;) {
    if(strcmp(temp->key, key) == 0) {
      strncpy(temp->value, value, MAX_VALUE_LENGTH);
      return 0;
    }

    if(temp->next == NULL) {
      temp->next = create_node(key, value);
      return 1;
    }

    temp = temp->next;
  }

  return 0;
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

static void reallocate(struct hash_map* map) {
  size_t new_capacity = map->capacity * 2;
  struct node** new_bucket_array = (struct node**) calloc(new_capacity, sizeof(struct node*));

  if(new_bucket_array == NULL) {
    return;
  }

  for(size_t i = 0; i < map->capacity; ++i) {
    struct node* head = map->bucket_array[i];

    while(head != NULL) {
      struct node* next = head->next;
      int new_hash = get_hash(head->key) % new_capacity;
      struct node** new_head = &new_bucket_array[new_hash];

      if(*new_head == NULL) {
        *new_head = head;
        (*new_head)->next = NULL;
      }
      else {
        struct node* temp = *new_head;
        *new_head = head;
        (*new_head)->next = temp;
      }

      head = next;
    }
  }

  free(map->bucket_array);
  map->bucket_array = new_bucket_array;
  map->capacity = new_capacity;
}

struct hash_map* hash_map_init(size_t size, float load_factor) {
  struct hash_map* map = (struct hash_map*) malloc(sizeof(struct hash_map));
  if(map == NULL) {
    return NULL;
  }

  map->bucket_array = (struct node**) calloc(size, sizeof(struct node*));
  if(map->bucket_array == NULL) {
    return NULL;
  }

  map->capacity = size;
  map->current_size = 0;
  map->load_factor = load_factor;
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
  float load_factor = (float) map->current_size / map->capacity;

  if(load_factor >= map->load_factor) {
    reallocate(map);
  }

  int hash = get_hash(key) % map->capacity;
  struct node* head = map->bucket_array[hash];

  int res = push_node(&head, key, value);
  if(res >= 0) {
    map->current_size += res;
  }
  else {
    printf("[ERR] Out of memory\n");
  }

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
    return NULL;
  }

  return found->value;
}

bool hash_map_delete(struct hash_map* map, const char* key) {
  int hash = get_hash(key) % map->capacity;
  struct node** head = &map->bucket_array[hash];

  bool res = delete_node(head, key);
  if(res) {
    map->current_size--;
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
