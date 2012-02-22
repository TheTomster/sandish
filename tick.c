#include <math.h>

#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include "board.h"
#include "cam.h"
#include "cursor.h"
#include "draw.h"
#include "matrix.h"
#include "tick.h"

#define MOVEMENT_DAMPING 0.05
#define ANGLE_DAMPING 0.001

#define FULLSCREEN

static board_handle board;
static cam_handle camera;
static cursor_handle cursor;

static void handle_inputs(void);

void tick_init(board_handle b, cam_handle c, cursor_handle cu) {
  board = b;
  camera = c;
  cursor = cu;
}

int tick(int swap) {
  draw_board();
  handle_inputs();
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

static void handle_inputs() {
  { // handle mouse movement
    int x, y;
    #ifdef FULLSCREEN
    glfwGetMousePos(&x, &y);
    glfwSetMousePos(0, 0);
    #else
    x = 0;
    y = 0;
    #endif

    cam_rotate(camera, y * ANGLE_DAMPING, x * ANGLE_DAMPING);
    cursor_update(cursor);
  }
  { // handle keyboard stuff
    GLfloat x = 0;
    GLfloat z = 0;

    if (glfwGetKey('A'))
      x += 1;
    if (glfwGetKey('D'))
      x += -1;
    if (glfwGetKey('W'))
      z += 1;
    if (glfwGetKey('S'))
      z += -1;

    x = x * MOVEMENT_DAMPING;
    z = z * MOVEMENT_DAMPING;

    vec4 trans_vec = (vec4) { x , 0, z, 1 };

    GLfloat rot_mat[16];
    cam_get_rot_mat(rot_mat, camera);
    GLfloat rot_mat_inv[16];
    matrix_invert(rot_mat_inv, rot_mat);

    trans_vec = vec4_mat_mult(rot_mat_inv, trans_vec);

    vec4 pos = cam_get_pos(camera);
    pos.x += trans_vec.x;
    pos.y += trans_vec.y;
    pos.z += trans_vec.z;
    cam_set_pos(camera, pos);
  }
  { // handle mouse clicks
    if (
        glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) &&
        cursor_in_board(cursor)) {
      vec4 cpos = cursor_get_board_pos(cursor);
      board_set_next(
          board, cpos.x, cpos.y, cpos.z, cursor_get_selected(cursor));
    }
  }
  { // handle keyboard block selection
    if (glfwGetKey('1'))
      cursor_set_selected(cursor, 1);
    if (glfwGetKey('2'))
      cursor_set_selected(cursor, 2);
    if (glfwGetKey('3'))
      cursor_set_selected(cursor, 3);
    if (glfwGetKey('4'))
      cursor_set_selected(cursor, 4);
  }
}
