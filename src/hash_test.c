#include "../include/hash_map.h"
#include <stdio.h>

void printif(const char* key, const char* value) {
  printf("This is key %s with value %s\n", key, value);
}

int main() {
  struct hash_map* map = hash_map_init(16);
  hash_map_put(map, "abc", "xyz");
  hash_map_put(map, "Aa", "1");
  hash_map_put(map, "BB", "2");
  printf("%s\n", hash_map_get(map, "abc"));
  printf("%s\n", hash_map_get(map, "Aa"));
  printf("%s\n", hash_map_get(map, "BB"));
  hash_map_iterate(map, printif);
  hash_map_put(map, "BB", "3");
  printf("%s\n", hash_map_get(map, "BB"));
  hash_map_put(map, "@\x80", "4");
  hash_map_delete(map, "BB");
  hash_map_delete(map, "Aa");
  hash_map_delete(map, "@\x80");
  printf("%s\n", hash_map_get(map, "abc"));
  const char* Aa = hash_map_get(map, "Aa");
  if(Aa) {
    printf("%s\n", Aa);
  }
  else {
    printf("Not found!");
  }
  printf("%s\n", hash_map_get(map, "BB"));
  hash_map_free(map);
  return 0;
}
