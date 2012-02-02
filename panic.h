#pragma once

#include <stdlib.h>
#include <stdio.h>

static void panic(char * message) {
  fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}
