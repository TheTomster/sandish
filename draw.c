
#include <GL3/gl3w.h>
#include <stdio.h>

#include "board.h"
#include "cam.h"
#include "draw.h"
#include "panic.h"
#include "screen.h"
#include "utils.h"

static screen_handle screen;
static GLuint position_buffer;
static unsigned int buffer_size;
static GLuint gshader;
static GLuint vshader;
static GLuint fshader;
static GLuint program;
static double cube_size;

static void init_pos_buffer(board_handle b);
static void init_shaders();

void draw_init(screen_handle s, board_handle b) {
  screen = s;
  init_pos_buffer(b);
  init_shaders();
}

void draw_board(board_handle b, cam_handle c) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUseProgram(program);

  // set cube size uniform
  GLint cube_size_uni = glGetUniformLocation(program, "cube_size");
  if (cube_size_uni == -1)
    printf("Warning: error setting cube size uniform.\n");
  glUniform1f(cube_size_uni, cube_size);

  // set aspect ratio uniform
  GLint aspect_uni = glGetUniformLocation(program, "aspect_ratio");
  if (aspect_uni == -1)
    printf("Warning: error setting aspect ratio uniform.\n");
  glUniform1f(aspect_uni, 1.0 / screen_aspect_ratio(screen));

  // Set up vertex positions
  GLint position_attrib = glGetAttribLocation(program, "position");
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(position_attrib);
  // Set up board data
  GLint data_attrib = glGetAttribLocation(program, "board_data");
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  const GLuint * board_data = board_get_data(b);
  glVertexAttribPointer(
      data_attrib, 1, GL_UNSIGNED_INT, GL_FALSE, 0, board_data);
  glEnableVertexAttribArray(data_attrib);

  glDrawArrays(GL_POINTS, 0, buffer_size);
}

static void init_pos_buffer(board_handle b) {
  vec4 origin = board_get_world_origin(b);
  cube_size = board_get_world_particle_step(b);
  unsigned int board_size = board_get_board_size(b);
  buffer_size = board_size * board_size * board_size;
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glBufferData(
      GL_ARRAY_BUFFER, buffer_size * sizeof(GLfloat) * 3,
      NULL, GL_STATIC_DRAW);
  float * map = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  if (map == NULL)
    panic("Unable to map buffer to application memory.");
  unsigned int i = 0;
  for (int x = 0; x < board_size; x++) {
    for (int y = 0; y < board_size; y++) {
      for (int z = 0; z < board_size; z++) {
        map[i] = origin.x + x * cube_size;
        map[i + 1] = origin.y + y * cube_size;
        map[i + 2] = origin.z + z * cube_size;
        i += 3;
      }
    }
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
}

static void init_shaders() {
  vshader = make_shader(GL_VERTEX_SHADER, "res/vertex.glsl");
  if (vshader == 0)
    panic("Failed to set up vertex shader.");
  gshader = make_shader(GL_GEOMETRY_SHADER, "res/geometry.glsl");
  if (gshader ==0)
    panic("Failed to set up geometry shader.");
  fshader = make_shader(GL_FRAGMENT_SHADER, "res/fragment.glsl");
  if (fshader == 0)
    panic("Failed to set up fragment shader.");
  program = make_program(gshader, vshader, fshader);
  if (program == 0)
    panic("Failed to link shaders.");
}
