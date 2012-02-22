#include "cursor.h"

#include <stdlib.h>
#include <math.h>

#include "board.h"
#include "cam.h"
#include "matrix.h"
#include "vec4.h"

#define CURSOR_DEPTH -4

struct cursor {
  vec4 board_pos;
  cam_handle camera;
  board_handle board;
  unsigned int selected;
};

cursor_handle cursor_new(cam_handle c, board_handle b) {
  cursor_handle ch = malloc(sizeof(struct cursor));
  ch->camera = c;
  ch->board = b;
  ch->selected = 1;
  return ch;
}

void cursor_delete(cursor_handle c) {
  free(c);
}

void cursor_update(cursor_handle c) {
  // make a cursor position in camera space, and convert it to world space
  vec4 cur_vec = (vec4){0, 0, CURSOR_DEPTH, 1};
  GLfloat cam_mat[16];
  cam_get_matrix(cam_mat, c->camera);
  GLfloat cam_mat_inv[16];
  matrix_invert(cam_mat_inv, cam_mat);
  cur_vec = vec4_mat_mult(cam_mat_inv, cur_vec);
  // use world space position of cursor to get board position
  vec4 origin = board_get_world_origin(c->board);
  GLfloat cube_size = board_get_world_particle_step(c->board);
  c->board_pos.x = floor((cur_vec.x - origin.x) / cube_size);
  c->board_pos.y = floor((cur_vec.y - origin.y) / cube_size);
  c->board_pos.z = floor((cur_vec.z - origin.z) / cube_size);
  c->board_pos.w = 1;
}

vec4 cursor_get_board_pos(cursor_handle c) {
  return c->board_pos;
}

int cursor_in_board(cursor_handle c) {
  unsigned int s = board_get_board_size(c->board);
  if (c->board_pos.x < 0 || c->board_pos.x > s - 1)
    return 0;
  if (c->board_pos.y < 0 || c->board_pos.y > s - 1)
    return 0;
  if (c->board_pos.z < 0 || c->board_pos.z > s - 1)
    return 0;
  return 1;
}

void cursor_set_selected(cursor_handle c, unsigned int id) {
  c->selected = id;
}

unsigned int cursor_get_selected(cursor_handle c) {
  return c->selected;
}
