
#include "cam.h"
#include "panic.h"
#include "vec4.h"

struct camera {
  vec4 pos, rotation;
};

cam_handle cam_new(vec4 pos, vec4 rotation) {
  cam_handle c = malloc(sizeof(struct camera));
  if (c == NULL)
    panic("Unable to allocate camera!");
  c->pos = pos;
  c->rotation = rotation;
  return c;
}

void cam_delete(cam_handle c) {
  free(c);
}

vec4 cam_get_pos(cam_handle c) {
  return c->pos;
}

void cam_set_pos(cam_handle c, vec4 newpos) {
  c->pos = newpos;
}
