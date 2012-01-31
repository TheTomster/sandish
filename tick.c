#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include "board.h"
#include "draw.h"
#include "tick.h"

static board_handle board;

void tick_init(board_handle b) {
  board = b;
}

int tick(int swap) {
  draw_board();
  glfwSwapBuffers();
  if (swap)
    board_swap_buffers(board);
  if (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)) {
    // not exiting
    return 0;
  } else {
    // exit
    return 1;
  }
}
