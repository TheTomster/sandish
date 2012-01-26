#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "board.h"
#include "lualink.h"
#include "panic.h"
#include "tick.h"

#define MAIN_FILE "lua/main.lua"

static lua_State *L;
static board_handle board;

// Wrapper declarations
static int l_board_get_existing(lua_State *L);
static int l_board_get_next(lua_State *L);
static int l_board_set_next(lua_State *L);
static luaL_Reg board_funcs[] = {
  {.name = "getE", .func = &l_board_get_existing},
  {.name = "getN", .func = &l_board_get_next},
  {.name = "setN", .func = &l_board_set_next},
  {.name = NULL, .func = NULL}
};
static int l_tick(lua_State *L);
static luaL_Reg tick_funcs[] = {
  {.name = "tick", .func = &l_tick},
  {.name = NULL, .func = NULL}
};

void lualink_init(board_handle b) {
  board = b;
  L = luaL_newstate();
  luaL_openlibs(L);
  luaL_newlib(L, board_funcs);
  lua_setglobal(L, "board");
  luaL_newlib(L, tick_funcs);
  lua_setglobal(L, "tick");
}

void lualink_load_main() {
  int err = luaL_dofile(L, MAIN_FILE);
  if (err != LUA_OK) {
    printf("%s\n", lua_tostring(L, -1));
    panic("Error loading main.lua");
  }
}

void lualink_enter_main() {
  lua_getglobal(L, "main");
  int err = lua_pcall(L, 0, 0, 0);
  if (err != LUA_OK) {
    printf("%s\n", lua_tostring(L, -1));
    panic("Error running lua's main()");
  }
}


// Function wrappers
static int l_board_get_existing(lua_State *L) {
  lua_Integer x,y,z;
  x = luaL_checkinteger(L, 1);
  y = luaL_checkinteger(L, 2);
  z = luaL_checkinteger(L, 3);
  GLuint result = board_get_existing(board, x, y, z);
  lua_pushnumber(L, result);
  return 1;
}

static int l_board_get_next(lua_State *L) {
  lua_Integer x,y,z;
  x = luaL_checkinteger(L, 1);
  y = luaL_checkinteger(L, 2);
  z = luaL_checkinteger(L, 3);
  GLuint result = board_get_next(board, x, y, z);
  lua_pushnumber(L, result);
  return 1;
}

static int l_board_set_next(lua_State *L) {
  lua_Integer x, y, z, d;
  x = luaL_checkinteger(L, 1);
  y = luaL_checkinteger(L, 2);
  z = luaL_checkinteger(L, 3);
  d = luaL_checkinteger(L, 4);
  board_set_next(board, x, y, z , d);
  return 0;
}

static int l_tick(lua_State *L) {
  int i = tick();
  lua_pushinteger(L, i);
  return 1;
}
