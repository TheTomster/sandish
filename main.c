#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include "board.h"
#include "cam.h"
#include "draw.h"
#include "lualink.h"
#include "panic.h"
#include "registry.h"
#include "screen.h"
#include "tick.h"

#define FULLSCREEN          0
#define WINDOW_WIDTH        800
#define WINDOW_HEIGHT       600
#define WINDOW_TITLE        "Sandish"

#define BOARD_WORLD_CENTER  {0, 0, -7, 1}
#define BOARD_WORLD_SIZE    5
#define BOARD_SIZE          32

//static void mainloop();
static void on_resize();

static screen_handle screen;

int main(void) {
  int ok = glfwInit();
  if (!ok)
    panic("Error initializing GLFW!");
  GLFWvidmode vm;
  glfwGetDesktopMode(&vm);
  if (FULLSCREEN) {
    ok = glfwOpenWindow(
        vm.Width, vm.Height, 0, 0, 0, 0, 8, 0, GLFW_FULLSCREEN);
  } else {
    ok = glfwOpenWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0, 0, 8, 0, GLFW_WINDOW);
  }
  if (!ok)
    panic ("Error opening window!");
  if (gl3wInit())
    panic("Gl3w failed to load!");
  glfwSetWindowTitle(WINDOW_TITLE);
  glfwSwapInterval(1);
  glfwSwapBuffers();

  glfwDisable(GLFW_MOUSE_CURSOR);
  glfwSetMousePos(0, 0);

  screen = screen_new();
  glfwSetWindowSizeCallback(&on_resize);

  cam_handle c;
  { // camera initialization
    vec4 pos = {0, 0, 0, 1};
    GLfloat rx = 0;
    GLfloat ry = 0;
    c = cam_new(pos, rx, ry);
  }

  board_handle b;
  { // board initialization
    vec4 center = BOARD_WORLD_CENTER;
    b = board_new(center, BOARD_WORLD_SIZE, BOARD_SIZE);
  }

  cursor_handle cu;
  cu = cursor_new(c, b);
  cursor_update(cu);

  registry_handle r;
  r = registry_new ();

  tick_init(b, c, cu);

  lualink_init(b, r);
  lualink_load_main();
  lualink_init_registry();

  draw_init(screen, b, c, r, cu);

  lualink_enter_main();

  board_delete(b);
  cam_delete(c);
  screen_delete(screen);
  glfwTerminate();
  return EXIT_SUCCESS;
}

static void on_resize(int width, int height) {
  screen_set_size(screen, width, height);
  glViewport(0, 0, width, height);
  draw_setup_perspective(screen_aspect_ratio(screen));
}
