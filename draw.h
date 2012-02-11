#pragma once

#include <GL3/gl3w.h>
#include <GL/glfw.h>

#include "board.h"
#include "cam.h"
#include "cursor.h"
#include "registry.h"
#include "screen.h"

void draw_init(
    screen_handle s, board_handle b, cam_handle c, registry_handle r,
    cursor_handle cu);

void draw_board();

void draw_setup_perspective(GLfloat aspect_ratio);
