#include <stdlib.h>

#include "list.h"
#include "registry.h"

struct registry {
  list_handle l;
};

registry_handle registry_new() {
  list_handle newlist = list_new();
  registry_handle r = malloc(sizeof(struct registry));
  r->l = newlist;
  return r;
}

void registry_delete(registry_handle r) {
  list_delete(r->l);
  free(r);
}

unsigned int registry_add(
    registry_handle r, const char * shader, size_t shader_len) {
  char * shader_copy = malloc(sizeof(char *) * shader_len + 1);
  for (int i = 0; i < shader_len; i++) {
    shader_copy[i] = shader[i];
  }
  shader_copy[shader_len] = '\0';
  list_push(r->l, shader_copy);
  return list_size(r->l);
}

const char * registry_get(registry_handle r, unsigned int index) {
  return list_get(r->l, index - 1);
}

unsigned int registry_size(registry_handle r) {
  return list_size(r->l);
}
