#pragma once

#include "board.h"
#include "cam.h"
#include "cursor.h"
#include "screen.h"

void tick_init(board_handle b, cam_handle c, cursor_handle cu, screen_handle s);

int tick(int swap);
