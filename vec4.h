// vec4 provides a basic vec4 for storing locations in 3d space.

#pragma once

#include <GL3/gl3w.h>
#include <GL/glfw.h>

typedef struct {
  GLfloat x, y, z, w;
} vec4;

void vec4_to_translation_matrix(GLfloat *r, vec4 v);

vec4 vec4_mat_mult(GLfloat * m, vec4 v);
