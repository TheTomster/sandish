// board represents the game board

#ifndef BOARD_H
#define BOARD_H

#include <GL3/gl3w.h>

#include "vec4.h"

typedef struct board* board_handle;

board_handle board_new(
    vec4 world_center, double world_size, unsigned int board_size);

void board_delete(board_handle b);

vec4 board_get_world_center(board_handle b);

double board_get_world_size(board_handle b);

vec4 board_get_world_origin(board_handle b);

double board_get_world_particle_step(board_handle b);

unsigned int board_get_board_size(board_handle b);

const GLuint * board_get_data(board_handle b);

void board_set_data(
    board_handle b, unsigned int x, unsigned int y, unsigned int z,
    unsigned int data);

#endif
