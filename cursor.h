#pragma once

#include "board.h"
#include "cam.h"

typedef struct cursor * cursor_handle;

cursor_handle cursor_new(cam_handle c, board_handle b);

void cursor_delete(cursor_handle c);

void cursor_update(cursor_handle c);

vec4 cursor_get_board_pos(cursor_handle c);

int cursor_in_board(cursor_handle c);

void cursor_set_selected(cursor_handle c, unsigned int id);

unsigned int cursor_get_selected(cursor_handle c);
