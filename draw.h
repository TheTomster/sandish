#ifndef DRAW_H
#define DRAW_H

#include "board.h"
#include "cam.h"
#include "registry.h"
#include "screen.h"

void draw_init(
    screen_handle s, board_handle b, cam_handle c, registry_handle r);

void draw_board();

#endif
