#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include "board.h"
#include "lualink.h"
#include "panic.h"
#include "registry.h"
#include "tick.h"

#define MAIN_FILE "lua/main.lua"

static lua_State *L;
static board_handle board;
static registry_handle registry;

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
static int l_tick_tick(lua_State *L);
static int l_tick_get_time(lua_State *L);
static luaL_Reg tick_funcs[] = {
  {.name = "tick", .func = &l_tick_tick},
  {.name = "getTime", .func = &l_tick_get_time},
  {.name = NULL, .func = NULL}
};
static int l_registry_add(lua_State *L);
static luaL_Reg registry_funcs[] = {
  {.name = "add", .func = &l_registry_add},
  {.name = NULL, .func = NULL}
};

// lualink functions
void lualink_init(board_handle b, registry_handle r) {
  board = b;
  registry = r;

  L = luaL_newstate();
  luaL_openlibs(L);
  luaL_newlib(L, board_funcs);
  lua_pushstring(L, "size");
  lua_pushnumber(L, board_get_board_size(board));
  lua_settable(L, 1);
  lua_setglobal(L, "board");
  luaL_newlib(L, tick_funcs);
  lua_setglobal(L, "tick");
  luaL_newlib(L, registry_funcs);
  lua_setglobal(L, "registry");
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

void lualink_init_registry() {
  lua_getglobal(L, "registry");
  lua_pushstring(L, "init");
  lua_gettable(L, 1);
  int err = lua_pcall(L, 0, 0, 0);
  if (err != LUA_OK) {
    printf("%s\n", lua_tostring(L, -1));
    panic("Error running lua's registry.init()");
  }
  lua_settop(L, 0);
}

// Function wrappers
static int l_board_get_existing(lua_State *L) {
  lua_Integer x,y,z;
  x = luaL_checkinteger(L, 1);
  y = luaL_checkinteger(L, 2);
  z = luaL_checkinteger(L, 3);
  // Subtract one from the indices -- so lua can be 1-indexed
  unsigned int result = board_get_existing(board, x - 1, y - 1, z - 1);
  lua_pushnumber(L, result);
  return 1;
}

static int l_board_get_next(lua_State *L) {
  lua_Integer x,y,z;
  x = luaL_checkinteger(L, 1);
  y = luaL_checkinteger(L, 2);
  z = luaL_checkinteger(L, 3);
  unsigned int result = board_get_next(board, x - 1, y - 1, z - 1);
  lua_pushnumber(L, result);
  return 1;
}

static int l_board_set_next(lua_State *L) {
  lua_Integer x, y, z, d;
  x = luaL_checkinteger(L, 1);
  y = luaL_checkinteger(L, 2);
  z = luaL_checkinteger(L, 3);
  d = luaL_checkinteger(L, 4);
  board_set_next(board, x - 1, y - 1, z - 1, d);
  return 0;
}

static int l_tick_tick(lua_State *L) {
  int swap = lua_toboolean(L, 1);
  int i = tick(swap);
  lua_settop(L, 0);
  lua_pushinteger(L, i);
  return 1;
}

static int l_tick_get_time(lua_State *L) {
  lua_pushnumber(L, glfwGetTime());
  return 1;
}

static int l_registry_add(lua_State *L) {
  const char * shader;
  size_t shader_len;
  shader = luaL_checklstring(L, 1, &shader_len);
  unsigned int data_value = registry_add(registry, shader, shader_len);
  lua_pushinteger(L, data_value);
  return 1;
}
