#version 150

uniform float cube_size;
uniform mat4 cam_matrix;
uniform mat4 perspective_matrix;

layout(points) in;
layout(triangle_strip, max_vertices=14) out;

in geo {
  float rand;
} i[];

out frag {
  float rand;
} o;

vec4 cube_strip[14] = vec4[14](
  perspective_matrix * cam_matrix * vec4( 1,  1,  1,  1),
  perspective_matrix * cam_matrix * vec4(-1,  1,  1,  1),
  perspective_matrix * cam_matrix * vec4( 1, -1,  1,  1),
  perspective_matrix * cam_matrix * vec4(-1, -1,  1,  1),
  perspective_matrix * cam_matrix * vec4(-1, -1, -1,  1),
  perspective_matrix * cam_matrix * vec4(-1,  1,  1,  1),
  perspective_matrix * cam_matrix * vec4(-1,  1, -1,  1),
  perspective_matrix * cam_matrix * vec4( 1,  1,  1,  1),
  perspective_matrix * cam_matrix * vec4( 1,  1, -1,  1),
  perspective_matrix * cam_matrix * vec4( 1, -1,  1,  1),
  perspective_matrix * cam_matrix * vec4( 1, -1, -1,  1),
  perspective_matrix * cam_matrix * vec4(-1, -1, -1,  1),
  perspective_matrix * cam_matrix * vec4(-1, -1,  1,  1),
  perspective_matrix * cam_matrix * vec4(-1,  1, -1,  1)
);

void main() {
  float cube_size_m = cube_size - 0.001;
  for (int x = 0; x < 14; x++) {
    gl_Position = gl_in[0].gl_Position + (cube_strip[x] * (cube_size_m / 2));
    o.rand = i[0].rand;
    EmitVertex();
  }
}
