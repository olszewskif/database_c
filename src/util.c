#include "../include/util.h"
#include <string.h>

void trim_newline(char* buffer) {
  size_t idx = strcspn(buffer, "\n");
  buffer[idx] = '\0';
}
