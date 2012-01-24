#version 150

uniform float cube_size;

layout(points) in;
layout(triangle_strip, max_vertices=14) out;

in geometry {
  flat uint board_data;
} g[];

out fragment {
  flat uint board_data;
} f;

vec4 cube_strip[14] = vec4[14](
  vec4(1, 1, 1, 1),
  vec4(0, 1, 1, 1),
  vec4(1, 0, 1, 1),
  vec4(0, 0, 1, 1),
  vec4(0, 0, 0, 1),
  vec4(0, 1, 1, 1),
  vec4(0, 1, 0, 1),
  vec4(1, 1, 1, 1),
  vec4(1, 1, 0, 1),
  vec4(1, 0, 1, 1),
  vec4(1, 0, 0, 1),
  vec4(0, 0, 0, 1),
  vec4(0, 0, 1, 1),
  vec4(0, 1, 0, 1)
);

void main() {
  for (int x = 0; x < 14; x++) {
    gl_Position = gl_in[0].gl_Position + (cube_strip[x] * cube_size);
    f.board_data = g[0].board_data;
    EmitVertex();
  }
}
