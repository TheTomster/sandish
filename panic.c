#include "panic.h"

void panic(char * message) {
  fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}
