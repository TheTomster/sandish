#pragma once

GLuint make_buffer(
    GLenum target, GLsizei buffer_size, const void * buffer_data,
    GLenum buffer_usage);

const char * load_file(const char * filename, GLint * length);

GLuint make_shader(GLenum type, const char * filename, int link_utils);

int compile_shader(GLuint shader);

// returns the name of a program linked from the given shader objects.
// if geom_shader is 0, it will be skipped.
GLuint make_program(
    GLuint vertex_shader, GLuint geom_shader, GLuint fragment_shader);
