#ifndef PANIC_H
#define PANIC_H

#include <stdlib.h>
#include <stdio.h>

static void panic(char * message) {
  fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}

#endif
