#pragma once

#include "board.h"
#include "registry.h"

void lualink_init(board_handle b, registry_handle r);

void lualink_load_main(void);

void lualink_enter_main(void);

void lualink_init_registry(void);
