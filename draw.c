#include <GL3/gl3w.h>
#include <stdio.h>

#include "board.h"
#include "cam.h"
#include "cursor.h"
#include "draw.h"
#include "list.h"
#include "panic.h"
#include "registry.h"
#include "screen.h"
#include "utils.h"

#define VERTEX_SHADER_FILE  "res/vertex.glsl"
#define GEO_SHADER_FILE     "res/geometry.glsl"
#define CURSOR_GSHADER_FILE "res/cursor.geo.glsl"
#define CURSOR_FSHADER_FILE "res/cursor.frag.glsl"

#define FRUSTUM_SCALE   1.0
#define FRUSTUM_NEAR    0.01
#define FRUSTUM_FAR     30.0

static screen_handle screen;
static board_handle board;
static cam_handle cam;
static registry_handle registry;
static cursor_handle cursor;

static list_handle buckets;

static GLuint position_buffer;
static unsigned int buffer_size;
static GLuint gshader;
static GLuint vshader;
static GLuint * fshaders;
static GLuint * programs;
static GLfloat cube_size;

static GLfloat perspective_matrix[16];

static void init_pos_buffer(board_handle b);
static void init_shaders();
static void update_buckets();
static void set_uniforms();

void draw_init(
    screen_handle s, board_handle b, cam_handle c, registry_handle r,
    cursor_handle cu) {
  screen = s;
  board = b;
  cam = c;
  registry = r;
  cursor = cu;
  init_pos_buffer(b);
  init_shaders();
}

void draw_board() {
  update_buckets();

  // convert the camera's orientation into a matrix
  GLfloat cam_matrix[16];
  cam_get_matrix(cam_matrix, cam);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  // For each bucket, draw the appropriate indices
  for (int i = 0; i < list_size(buckets); i++) {
    // select appropriate shader program
    GLuint program = programs[i];
    glUseProgram(program);
    set_uniforms(program, cam_matrix);
    // Set up vertex positions
    GLint position_attrib = glGetAttribLocation(program, "position");
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(position_attrib);
    // Get the list of indices
    list_handle is = list_get(buckets, i);
    // Set up a buffer of the correct size
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * list_size(is), NULL,
        GL_STREAM_DRAW);
    // Fill the buffer with indices
    GLuint * bufmap = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int j = 0; j < list_size(is); j++) {
      unsigned int * i_ptr = (unsigned int *) list_get(is, j);
      bufmap[j] = *i_ptr;
    }
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    // Draw that buffer data
    glDrawElements(GL_POINTS, list_size(is), GL_UNSIGNED_INT, 0);
    // Delete that buffer
    glDeleteBuffers(1, &index_buffer);
  }
  // draw cursor
  if (cursor_in_board(cursor)) {
    // get position index from cursor board position
    vec4 cur_pos = cursor_get_board_pos(cursor);
    unsigned int index = board_get_index(board, cur_pos.x, cur_pos.y, cur_pos.z);
    // Draw a cube with that index
    GLuint cursor_program = programs[cursor_get_selected(cursor) - 1];
    glUseProgram(cursor_program);
    set_uniforms(cursor_program, cam_matrix);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    GLint position_attrib = glGetAttribLocation(cursor_program, "position");
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(position_attrib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, &index);
  }
}

void draw_setup_perspective(GLfloat aspect_ratio) {
  for (int i = 0; i < 16; i++) {
    GLfloat x;
    switch (i) {
      case 0:
        x = FRUSTUM_SCALE / aspect_ratio;
        break;
      case 5:
        x = FRUSTUM_SCALE;
        break;
      case 10:
        x = (FRUSTUM_FAR + FRUSTUM_NEAR) / (FRUSTUM_NEAR - FRUSTUM_FAR);
        break;
      case 11:
        x = -1;
        break;
      case 14:
        x = 2 * FRUSTUM_FAR * FRUSTUM_NEAR / (FRUSTUM_NEAR - FRUSTUM_FAR);
        break;
      default:
        x = 0;
        break;
    }
    perspective_matrix[i] = x;
  }
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
  GLfloat * map = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
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
  vshader = make_shader(GL_VERTEX_SHADER, VERTEX_SHADER_FILE);
  if (vshader == 0)
    panic("Failed to set up vertex shader.");
  gshader = make_shader(GL_GEOMETRY_SHADER, GEO_SHADER_FILE);
  if (gshader == 0)
    panic("Failed to set up geometry shader.");
  // load and compile each fragment shader, build a program from it, and add
  // that program to the list.
  fshaders = malloc(sizeof(GLuint) * registry_size(registry));
  programs = malloc(sizeof(GLuint) * registry_size(registry));
  for (int i = 0; i < registry_size(registry); i++) {
    fshaders[i] = make_shader(
        GL_FRAGMENT_SHADER, registry_get(registry, i + 1));
    programs[i] = make_program(vshader, gshader, fshaders[i]);
    if (programs[i] == 0)
      panic("Failed to create program.");
  }
}

static void update_buckets() {
  // Create a bucket for each possible data value.
  if (buckets != NULL) {
    for (int i = 0; i < list_size(buckets); i++) {
      list_delete(list_get(buckets, i));
    }
    list_delete(buckets);
    buckets = NULL;
  }
  buckets = list_new();
  unsigned int num_buckets = registry_size(registry);
  for (int i = 0; i < num_buckets; i++) {
    list_handle bucket = list_new();
    list_push(buckets, bucket);
  }
  // Scan the board and add indices to each bucket.
  const unsigned int * board_buffer = board_get_buffer(board);
  for (int i = 0; i < board_get_max_index(board); i++) {
    unsigned int value = board_buffer[i];
    if (value != 0) {
      list_handle bucket = list_get(buckets, value - 1);
      unsigned int * i_entry = malloc(sizeof(unsigned int));
      *i_entry = i;
      list_push(bucket, i_entry);
    }
  }
}

static void set_uniforms(GLuint program, GLfloat * cam_matrix) {
  // set perspective matrix uniform
  GLint persp_mat_uni = glGetUniformLocation(program, "perspective_matrix");
  if (persp_mat_uni == -1) {
    printf("Warning: error setting perspective matrix uniform.\n");
  }
  glUniformMatrix4fv(persp_mat_uni, 1, GL_FALSE, perspective_matrix);
  // set camera matrix uniform
  GLint cam_mat_uni = glGetUniformLocation(program, "cam_matrix");
  if (cam_mat_uni == -1)
    printf("Warning: error setting cube size uniform.\n");
  glUniformMatrix4fv(cam_mat_uni, 1, GL_FALSE, cam_matrix);
  // set cube size uniform
  GLint cube_size_uni = glGetUniformLocation(program, "cube_size");
  if (cube_size_uni == -1)
    printf("Warning: error setting cube size uniform.\n");
  glUniform1f(cube_size_uni, cube_size);
}
