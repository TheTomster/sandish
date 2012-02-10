// board represents the game board

#pragma once

#include "vec4.h"

typedef struct board* board_handle;

board_handle board_new(
    vec4 world_center, GLfloat world_size, unsigned int board_size);

void board_delete(board_handle b);

vec4 board_get_world_center(board_handle b);

GLfloat board_get_world_size(board_handle b);

vec4 board_get_world_origin(board_handle b);

GLfloat board_get_world_particle_step(board_handle b);

unsigned int board_get_board_size(board_handle b);

unsigned int board_get_max_index(board_handle b);

const unsigned int * board_get_buffer(board_handle b);

const unsigned int board_get_existing(
    board_handle b, unsigned int x, unsigned int y, unsigned int z);

const unsigned int board_get_next(
    board_handle b, unsigned int x, unsigned int y, unsigned int z);

void board_set_next(
    board_handle b, unsigned int x, unsigned int y, unsigned int z,
    unsigned int data);

void board_swap_buffers(board_handle b);
