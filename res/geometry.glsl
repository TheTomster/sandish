#version 150

uniform float cube_size;
uniform float aspect_ratio;

layout(points) in;
layout(triangle_strip, max_vertices=14) out;

in geo {
  float depth;
} i[];

out frag {
  float depth;
} o;

vec4 cube_strip[14] = vec4[14](
  vec4(1 * aspect_ratio, 1, 1, 1),
  vec4(0 * aspect_ratio, 1, 1, 1),
  vec4(1 * aspect_ratio, 0, 1, 1),
  vec4(0 * aspect_ratio, 0, 1, 1),
  vec4(0 * aspect_ratio, 0, 0, 1),
  vec4(0 * aspect_ratio, 1, 1, 1),
  vec4(0 * aspect_ratio, 1, 0, 1),
  vec4(1 * aspect_ratio, 1, 1, 1),
  vec4(1 * aspect_ratio, 1, 0, 1),
  vec4(1 * aspect_ratio, 0, 1, 1),
  vec4(1 * aspect_ratio, 0, 0, 1),
  vec4(0 * aspect_ratio, 0, 0, 1),
  vec4(0 * aspect_ratio, 0, 1, 1),
  vec4(0 * aspect_ratio, 1, 0, 1)
);

void main() {
  for (int x = 0; x < 14; x++) {
    gl_Position = gl_in[0].gl_Position + (cube_strip[x] * cube_size);
    o.depth = i[0].depth;
    EmitVertex();
  }
}
