#pragma once

#include "board.h"
#include "cam.h"
#include "cursor.h"

void tick_init(board_handle b, cam_handle c, cursor_handle cu);

int tick(int swap);
