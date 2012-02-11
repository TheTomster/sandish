#include <GL3/gl3w.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

GLuint make_buffer(
    GLenum target, GLsizei buffer_size, const void * buffer_data,
    GLenum buffer_usage) {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, buffer_size, buffer_data, buffer_usage);
  if (buffer == 0) {
    fprintf(stderr, "Error loading buffer");
    return 0;
  }
  return buffer;
}

const char * load_file(const char * filename, GLint * length) {
  FILE * fd = fopen(filename, "r");
  if (fd == 0) {
    fprintf(stderr, "Unable to open %s for reading.\n", filename);
    return NULL;
  }
  fseek(fd, 0, SEEK_END);
  *length = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  char * buffer = malloc(*length + 1);
  *length = fread(buffer, 1, *length, fd);
  fclose(fd);
  buffer[*length] = '\0';
  return buffer;
}

GLuint make_shader(GLenum type, const char * filename) {
  GLuint shader;
  shader = glCreateShader(type);

  GLint source_len;
  const char * source = load_file(filename, &source_len);
  glShaderSource(shader, 1, &source, NULL);
  int ok = compile_shader(shader);
  if (!ok)
    return 0;
  return shader;
}

int compile_shader(GLuint shader) {
  glCompileShader(shader);

  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    fprintf(stderr, "Failed to compile shader:\n");
    GLint log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    char * log = malloc(log_length + 1);
    glGetShaderInfoLog(shader, log_length + 1, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
    glDeleteShader(shader);
    return 0;
  }
  return 1;
}

GLuint make_program(
    GLuint geom_shader, GLuint vertex_shader, GLuint fragment_shader) {
  GLuint program = glCreateProgram();
  if (program == 0) {
    fprintf(stderr, "Error creating program\n");
    return 0;
  }
  if (geom_shader != 0) {
    glAttachShader(program, geom_shader);
  }
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  GLint linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    fprintf(stderr, "Failed to link program\n");
    GLint log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    char * log = malloc(log_length + 1);
    glGetProgramInfoLog(program, log_length + 1, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
    glDeleteProgram(program);
    return 0;
  }
  return program;
}

