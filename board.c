// board represents the game board

#include <stdlib.h>

#include "board.h"
#include "panic.h"

static unsigned int get_index(
    board_handle b, unsigned int x, unsigned int y, unsigned int z);

struct board {
  vec4 world_center; // world coords of centerpoint
  double world_size;
  // number of cells in the board. (a value of 30 here makes a 30x30x30 cube)
  unsigned int board_size;
  unsigned int max_index;
  vec4 world_origin;
  unsigned int *data_e;
  unsigned int *data_n;
};

board_handle board_new(
    vec4 world_center, double world_size, unsigned int board_size) {
  board_handle b = malloc(sizeof(struct board));
  if (b == NULL)
    panic("Error allocating board memory!");
  b->world_center = world_center;
  b->world_size = world_size;
  b->board_size = board_size;
  b->max_index = board_size * board_size * board_size;
  b->data_e = malloc(sizeof(unsigned int) * b->max_index);
  b->data_n = malloc(sizeof(unsigned int) * b->max_index);
  if (b->data_e == NULL || b->data_n == NULL)
    panic("Error allocating board data!");
  for (int i = 0; i < b->max_index; i++) {
    b->data_e[i] = 0;
    b->data_n[i] = 0;
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
  return b;
}

void board_delete(board_handle b) {
  free(b->data_e);
  free(b->data_n);
  free(b);
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

unsigned int board_get_max_index(board_handle b) {
  unsigned int x = b->board_size;
  return x * x * x;
}

const unsigned int * board_get_buffer(board_handle b) {
  return b->data_e;
}

const unsigned int board_get_existing(
    board_handle b, unsigned int x, unsigned int y, unsigned int z) {
  unsigned int i = get_index(b, x, y, z);
  return b->data_e[i];
}

const unsigned int board_get_next(
    board_handle b, unsigned int x, unsigned int y, unsigned int z) {
  unsigned int i = get_index(b, x, y, z);
  return b->data_n[i];
}

void board_set_next(
    board_handle b, unsigned int x, unsigned int y, unsigned int z,
    unsigned int data) {
  unsigned int i = get_index(b, x, y, z);
  b->data_n[i] = data;
}

void board_swap_buffers(board_handle b) {
  unsigned int * swap = b->data_e;
  b->data_e = b->data_n;
  b->data_n = swap;
  int x = b->board_size;
  int max_index = x * x * x;
  for (int i = 0; i < max_index; i++) {
    b->data_n[i] = b->data_e[i];
  }
}

static unsigned int get_index(
    board_handle b, unsigned int x, unsigned int y, unsigned int z) {
  unsigned int bs = b->board_size;
  unsigned int i =  x * bs * bs + y * bs + z;
  if (i > b->max_index - 1)
    panic("Buffer overrun!");
  return i;
}
