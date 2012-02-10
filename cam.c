#include <math.h>

#include "cam.h"
#include "matrix.h"
#include "panic.h"
#include "vec4.h"

#define PI 3.14159
#define POS_X_ROT_CLAMP (PI / 2)
#define NEG_X_ROT_CLAMP (-PI / 2)
#define Y_ROT_CLAMP (2 * PI)

struct camera {
  vec4 pos;
  GLfloat rx, ry;
};

cam_handle cam_new(vec4 pos, GLfloat rx, GLfloat ry) {
  cam_handle c = malloc(sizeof(struct camera));
  if (c == NULL)
    panic("Unable to allocate camera!");
  c->pos = pos;
  c->rx = rx;
  c->ry = ry;
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

GLfloat cam_get_rx(cam_handle c) {
  return c->rx;
}

GLfloat cam_get_ry(cam_handle c) {
  return c->ry;
}

void cam_get_rot_mat(GLfloat * r, cam_handle c) {
  GLfloat rx = c->rx;
  GLfloat cam_rot_x[16] = {
    1, 0,        0,       0,
    0, cos(rx),  sin(rx), 0,
    0, -sin(rx), cos(rx), 0,
    0, 0,        0,       1
  };
  GLfloat ry = c->ry;
  GLfloat cam_rot_y[16] = {
    cos(ry), 0,    -sin(ry), 0,
    0,       1,    0,        0,
    sin(ry), 0,    cos(ry),  0,
    0,       0,    0,        1
  };
  matrix_multiply(r, cam_rot_x, cam_rot_y);
}

void cam_get_matrix(GLfloat * r, cam_handle c) {
  GLfloat cam_rot[16];
  cam_get_rot_mat(cam_rot, c);
  GLfloat cam_pos[16];
  vec4_to_translation_matrix(cam_pos, c->pos);
  matrix_multiply(r, cam_rot, cam_pos);
}

void cam_rotate(cam_handle c, GLfloat dx, GLfloat dy) {
  c->rx += dx;
  c->ry += dy;
  if (c->rx > POS_X_ROT_CLAMP)
    c->rx = POS_X_ROT_CLAMP;
  if (c->rx < NEG_X_ROT_CLAMP)
    c->rx = NEG_X_ROT_CLAMP;
  if (c->ry > Y_ROT_CLAMP)
    c->ry -= Y_ROT_CLAMP;
}
