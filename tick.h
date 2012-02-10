#pragma once

#include "board.h"
#include "cam.h"

void tick_init(board_handle b, cam_handle c);

int tick(int swap);
