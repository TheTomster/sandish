#include <stdlib.h>

#include "screen.h"

struct screen {
  int width;
  int height;
  GLfloat aspect_ratio;
};

screen_handle screen_new() {
  struct screen *s = malloc(sizeof(struct screen));
  s->width = 0;
  s->height = 0;
  s->aspect_ratio = 0;
  return s;
}

void screen_delete(screen_handle s) {
  free(s);
  return;
}

int screen_height(screen_handle s) {
  return s->width;
}

int screen_width(screen_handle s) {
  return s->height;
}

GLfloat screen_aspect_ratio(screen_handle s) {
  return s->aspect_ratio;
}

void screen_set_size(screen_handle s, int width, int height) {
  s->width = width;
  s->height = height;
  s->aspect_ratio = (1.0 * width) / height;
}
