
#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include "board.h"
#include "cam.h"
#include "draw.h"
#include "tick.h"

static board_handle board;
static cam_handle cam;

void tick_init(board_handle b, cam_handle c) {
  board = b;
  cam = c;
}

int tick() {
  draw_board(board, cam);
  glfwSwapBuffers();
  if (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)) {
    return 0;
  } else {
    return 1;
  }
}
