#include <stdlib.h>

#include "screen.h"

struct screen {
  int width;
  int height;
  int fullscreen;
  int mid_x;
  int mid_y;
  GLfloat aspect_ratio;
};

screen_handle screen_new() {
  struct screen *s = malloc(sizeof(struct screen));
  s->width = 0;
  s->height = 0;
  s->mid_x = 0;
  s->mid_y = 0;
  s->fullscreen = 0;
  s->aspect_ratio = 0;
  return s;
}

void screen_delete(screen_handle s) {
  free(s);
  return;
}

int screen_height(screen_handle s) {
  return s->height;
}

int screen_width(screen_handle s) {
  return s->width;
}

GLfloat screen_aspect_ratio(screen_handle s) {
  return s->aspect_ratio;
}

void screen_set_size(screen_handle s, int width, int height) {
  s->width = width;
  s->height = height;
  s->mid_x = width / 2;
  s->mid_y = height / 2;
  s->aspect_ratio = (1.0 * width) / height;
}

void screen_set_fullscreen(screen_handle s, int is_fullscreen) {
  s->fullscreen = is_fullscreen;
}

int screen_fullscreen(screen_handle s) {
  return s->fullscreen;
}

void screen_midpoint(int * x, int * y, screen_handle s) {
  *x = s->mid_x;
  *y = s->mid_y;
}
