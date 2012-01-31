#ifndef LUALINK_H
#define LUALINK_H

#include "board.h"
#include "registry.h"

void lualink_init(board_handle b, registry_handle r);

void lualink_load_main();

void lualink_enter_main();

void lualink_init_registry();

#endif
