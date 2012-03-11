#include <GL3/gl3w.h>
#include <stdio.h>

#include "tgl/tgl.h"

#include "board.h"
#include "cam.h"
#include "cursor.h"
#include "draw.h"
#include "list.h"
#include "panic.h"
#include "registry.h"
#include "screen.h"

#define FRUSTUM_SCALE   1.0
#define FRUSTUM_NEAR    0.01
#define FRUSTUM_FAR     30.0

static const char * VERTEX_SHADER_FILE = "res/vertex.glsl";
static const char * GEO_SHADER_FILE    = "res/geometry.glsl";
static const char * SSAO_VSHADER_FILE  = "res/ssao/vertex.glsl";
static const char * SSAO_FSHADER_FILE  = "res/ssao/fragment.glsl";
static const char * FSHADER_UTILS_FILE = "res/util.frag.glsl";

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

static GLuint ssao_vert_buffer;
static GLfloat ssao_vertices[8] = {
  -1, -1,
  -1, 1,
  1, -1,
  1, 1,
};
static GLuint ssao_vshader;
static GLuint ssao_fshader;
static GLuint ssao_program;

static GLfloat perspective_matrix[16];

static GLuint geo_tex;
static GLuint pos_tex;
static GLuint norm_tex;
static GLuint mrt_fb;
static GLuint mrt_depthbuffer;

static void init_pos_buffer(board_handle b);
static void init_ssao_buffer(void);
static void init_shaders(void);
static void init_mrt(void);
static void update_buckets(void);
static void set_uniforms(
    GLuint program, GLfloat *cam_matrix, GLfloat *cam_rot_matrix);

void draw_init(
    screen_handle s, board_handle b, cam_handle c, registry_handle r,
    cursor_handle cu) {
  screen = s;
  board = b;
  cam = c;
  registry = r;
  cursor = cu;
  init_pos_buffer(b);
  init_ssao_buffer();
  init_shaders();
  init_mrt();
}

void draw_board() {
  update_buckets();

  // convert the camera's orientation into a matrix
  GLfloat cam_matrix[16];
  cam_get_matrix(cam_matrix, cam);
  GLfloat cam_rot_matrix[16];
  cam_get_rot_mat(cam_rot_matrix, cam);

  glBindFramebuffer(GL_FRAMEBUFFER, mrt_fb);
  GLuint drawbuffers[3] = {
      GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, drawbuffers);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  // For each bucket, draw the appropriate indices
  for (int i = 0; i < list_size(buckets); i++) {
    // select appropriate shader program
    GLuint program = programs[i];
    glUseProgram(program);
    set_uniforms(program, cam_matrix, cam_rot_matrix);
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
    set_uniforms(cursor_program, cam_matrix, cam_rot_matrix);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    GLint position_attrib = glGetAttribLocation(cursor_program, "position");
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(position_attrib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, &index);
  }

  // Switch to screen rendering
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  { // Do another pass to render SSAO to screen
    // set up program
    glUseProgram(ssao_program);

    // set up vertices
    glBindBuffer(GL_ARRAY_BUFFER, ssao_vert_buffer);
    GLint position_attrib = glGetAttribLocation(ssao_program, "position");
    glVertexAttribPointer(position_attrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(position_attrib);

    // set up textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, geo_tex);
    GLint geo_samp_uni = glGetUniformLocation(ssao_program, "geo_samp");
    glUniform1i(geo_samp_uni, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pos_tex);
    GLint pos_samp_uni = glGetUniformLocation(ssao_program, "pos_samp");
    glUniform1i(pos_samp_uni, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, norm_tex);
    GLint norm_samp_uni = glGetUniformLocation(ssao_program, "norm_samp");
    glUniform1i(norm_samp_uni, 2);

    // do the draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
  {
    tgl_shaderset ss = {&VERTEX_SHADER_FILE, 1};
    vshader = tgl_make_shader(GL_VERTEX_SHADER, ss);
  }
  if (vshader == 0)
    panic("Failed to set up vertex shader.");
  {
    tgl_shaderset ss = {&GEO_SHADER_FILE, 1};
    gshader = tgl_make_shader(GL_GEOMETRY_SHADER, ss);
  }
  if (gshader == 0)
    panic("Failed to set up geometry shader.");
  // load and compile each fragment shader, build a program from it, and add
  // that program to the list.
  fshaders = malloc(sizeof(GLuint) * registry_size(registry));
  programs = malloc(sizeof(GLuint) * registry_size(registry));
  for (int i = 0; i < registry_size(registry); i++) {
    {
      tgl_shaderset ss;
      const char * filenames[2];
      filenames[0] = FSHADER_UTILS_FILE;
      filenames[1] = registry_get(registry, i + 1);
      ss.filenames = filenames;
      ss.nfiles = 2;
      fshaders[i] = tgl_make_shader(GL_FRAGMENT_SHADER, ss);
    }
    programs[i] = tgl_make_program(vshader, gshader, fshaders[i]);
    if (programs[i] == 0)
      panic("Failed to create program.");
  }

  // set up ssao shaders
  {
    tgl_shaderset ss = {&SSAO_VSHADER_FILE, 1};
    ssao_vshader = tgl_make_shader(GL_VERTEX_SHADER, ss);
  }
  if (ssao_vshader == 0)
    panic("Failed to set up SSAO vertex shader.");
  {
    tgl_shaderset ss = {&SSAO_FSHADER_FILE, 1};
    ssao_fshader = tgl_make_shader(GL_FRAGMENT_SHADER, ss);
  }
  if (ssao_fshader == 0)
    panic("Failed to set up SSAO fragment shader.");
  ssao_program = tgl_make_program(ssao_vshader, 0, ssao_fshader);
  if (ssao_program == 0)
    panic("Failed to link SSAO program.");
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

static void set_uniforms(
    GLuint program, GLfloat * cam_matrix, GLfloat * cam_rot_matrix) {
  // set perspective matrix uniform
  GLint persp_mat_uni = glGetUniformLocation(program, "perspective_matrix");
  if (persp_mat_uni == -1)
    printf("Warning: error setting perspective matrix uniform.\n");
  glUniformMatrix4fv(persp_mat_uni, 1, GL_FALSE, perspective_matrix);
  // set camera matrix uniform
  GLint cam_mat_uni = glGetUniformLocation(program, "cam_matrix");
  if (cam_mat_uni == -1)
    printf("Warning: error setting camera matrix uniform.\n");
  glUniformMatrix4fv(cam_mat_uni, 1, GL_FALSE, cam_matrix);
  // set rotation matrix uniform
  GLint cam_mat_rot_uni = glGetUniformLocation(program, "cam_rot_matrix");
  if (cam_mat_rot_uni == -1)
    printf("Warning: error setting camera rotation uniform.\n");
  glUniformMatrix4fv(cam_mat_rot_uni, 1, GL_FALSE, cam_rot_matrix);
  // set cube size uniform
  GLint cube_size_uni = glGetUniformLocation(program, "cube_size");
  if (cube_size_uni == -1)
    printf("Warning: error setting cube size uniform.\n");
  glUniform1f(cube_size_uni, cube_size);
}

static void init_mrt() {
  // Create textures
  GLuint * ptrs[3] = {&geo_tex, &pos_tex, &norm_tex};
  int width = screen_width(screen);
  int height = screen_height(screen);
  char * texbuf = malloc(sizeof(char) * 3 * width * height);
  for (int i = 0; i < 3; i++) {
    GLuint * tex = ptrs[i];
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
        texbuf);
  }
  free(texbuf);

  // Set up framebuffer object
  glGenFramebuffers(1, &mrt_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, mrt_fb);
  // add textures to fbo
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, geo_tex, 0);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, pos_tex, 0);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, norm_tex, 0);
  // add a depthbuffer to fbo
  glGenRenderbuffers(1, &mrt_depthbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, mrt_depthbuffer);
  glRenderbufferStorage(
      GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(
      GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mrt_depthbuffer);
}

static void init_ssao_buffer() {
  glGenBuffers(1, &ssao_vert_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, ssao_vert_buffer);
  glBufferData(
      GL_ARRAY_BUFFER, 4 * sizeof(GLfloat) * 3, ssao_vertices, GL_STATIC_DRAW);
}
