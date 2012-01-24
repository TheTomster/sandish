// board represents the game board

#include <stdlib.h>

#include "board.h"
#include "panic.h"

struct board {
  vec4 world_center; // world coords of centerpoint
  double world_size;
  // number of cells in the board. (a value of 30 here makes a 30x30x30 cube)
  unsigned int board_size;
  vec4 world_origin;
  GLuint *data;
};

board_handle board_new(
    vec4 world_center, double world_size, unsigned int board_size) {
  board_handle b = malloc(sizeof(struct board));
  if (b == NULL)
    panic("Error allocating board memory!");
  b->world_center = world_center;
  b->world_size = world_size;
  b->board_size = board_size;
  b->data = malloc(
      sizeof(GLuint) * board_size * board_size * board_size);
  for (int i = 0; i < board_size * board_size * board_size; i++) {
    b->data[i] = 0;
  }
  // compute the mapping from 0,0,0 in board coords to world coords
  {
    double x, y, z;
    double halfsize = b->world_size / 2;
    x = b->world_center.x - halfsize;
    y = b->world_center.y - halfsize;
    z = b->world_center.z - halfsize;
    vec4 v = {x, y, z, 1};
    b->world_origin = v;
  }
  if (b->data == NULL)
    panic("Error allocating board data!");
  return b;
}

void board_delete(board_handle b) {
  free(b);
  free(b->data);
}

vec4 board_get_world_center(board_handle b) {
  return b->world_center;
}

double board_get_world_size(board_handle b) {
  return b->world_size;
}

vec4 board_get_world_origin(board_handle b) {
  return b->world_origin;
}

double board_get_world_particle_step(board_handle b) {
  return b->world_size / b->board_size;
}

unsigned int board_get_board_size(board_handle b) {
  return b->board_size;
}

const GLuint * board_get_data(board_handle b) {
  return (const GLuint *) b->data;
}

void board_set_data(
    board_handle b, unsigned int x, unsigned int y, unsigned int z,
    GLuint data) {
}
