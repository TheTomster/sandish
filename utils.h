#ifndef UTILS__H
#define UTILS__H

GLuint make_buffer(
    GLenum target, GLsizei buffer_size, const void * buffer_data,
    GLenum buffer_usage);

const char * load_file(const char * filename, GLint * length);

GLuint make_shader(GLenum type, const char * filename);

GLuint make_program(
    GLuint geom_shader, GLuint vertex_shader, GLuint fragment_shader);

#endif
