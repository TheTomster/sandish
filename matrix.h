#pragma once

#include <GL3/gl3w.h>
#include <GL/glfw.h>

void matrix_multiply(GLfloat *r, GLfloat *a, GLfloat *b);

void matrix_invert(GLfloat *r, GLfloat *m);
