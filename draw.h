#ifndef DRAW_H
#define DRAW_H

#include "board.h"
#include "screen.h"
#include "cam.h"

void draw_init(screen_handle s, board_handle b);

void draw_board(board_handle b, cam_handle c);

#endif
