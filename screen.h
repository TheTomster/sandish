#pragma once

#include <GL3/gl3w.h>
#include <GL/glfw.h>

typedef struct screen* screen_handle;

screen_handle screen_new(void);

void screen_delete(screen_handle s);

int screen_height(screen_handle s);

int screen_width(screen_handle s);

GLfloat screen_aspect_ratio(screen_handle s);

void screen_set_size(screen_handle s, int width, int height);
