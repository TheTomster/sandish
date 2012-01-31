#include <stdlib.h>

#include "list.h"

#define DEFAULT_CAP 32

struct list {
  void * * buffer;
  unsigned int size;
  unsigned int cap;
};

list_handle list_new() {
  list_handle l = malloc(sizeof(struct list));
  l->buffer = malloc(sizeof(void * *) * DEFAULT_CAP);
  l->size = 0;
  l->cap = DEFAULT_CAP;
  return l;
}

void list_delete(list_handle l) {
  for (int i = 0; i < l->size; i++) {
    free(l->buffer[i]);
  }
  free(l->buffer);
  free(l);
}

void list_push(list_handle l, void * item) {
  if (l->size == l->cap) {
    l->cap = l->cap * 2;
    void * * new_buffer = malloc(sizeof(void * *) * l->cap);
    for (int i = 0; i < l->size; i++) {
      new_buffer[i] = l->buffer[i];
    }
    free(l->buffer);
    l->buffer = new_buffer;
  }
  l->buffer[l->size] = item;
  l->size = l->size + 1;
}

void * list_get(list_handle l, unsigned int index) {
  if (index > l->size - 1)
    return NULL;
  return l->buffer[index];
}

unsigned int list_size(list_handle l) {
  return l->size;
}
