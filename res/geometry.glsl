#version 150

uniform float cube_size;
uniform mat4 cam_matrix;
uniform mat4 perspective_matrix;
uniform mat4 cam_rot_matrix;

layout(points) in;
layout(triangle_strip, max_vertices=42) out;

in geo {
  float rand;
} i[];

out frag {
  float rand;
} o;

out fragutil {
  smooth vec4 pos;
  flat vec4 normal;
} ofu;

vec4 cube_triangles[42] = vec4[42](
  //front
  vec4(-1, 1, -1, 1),
  vec4(-1, -1, -1, 1),
  vec4(1, 1, -1, 1),
  vec4(1, 1, -1, 1),
  vec4(-1, -1, -1, 1),
  vec4(1, -1, -1, 1),
  vec4(0, 0, 0, 1),
  // back
  vec4(-1, 1, 1, 1),
  vec4(1, 1, 1, 1),
  vec4(-1, -1, 1, 1),
  vec4(1, 1, 1, 1),
  vec4(1, -1, 1, 1),
  vec4(-1, -1, 1, 1),
  vec4(0, 0, 0, 1),
  // left
  vec4(-1, 1, -1, 1),
  vec4(-1, -1, 1, 1),
  vec4(-1, -1, -1, 1),
  vec4(-1, 1, -1, 1),
  vec4(-1, 1, 1, 1),
  vec4(-1, -1, 1, 1),
  vec4(0, 0, 0, 1),
  // right
  vec4(1, 1, -1, 1),
  vec4(1, -1, -1, 1),
  vec4(1, -1, 1, 1),
  vec4(1, 1, -1, 1),
  vec4(1, -1, 1, 1),
  vec4(1, 1, 1, 1),
  vec4(0, 0, 0, 1),
  // top
  vec4(-1, 1, 1, 1),
  vec4(-1, 1, -1, 1),
  vec4(1, 1, 1, 1),
  vec4(1, 1, 1, 1),
  vec4(-1, 1, -1, 1),
  vec4(1, 1, -1, 1),
  vec4(0, 0, 0, 1),
  // bottom
  vec4(-1, -1, 1, 1),
  vec4(1, -1, 1, 1),
  vec4(-1, -1, -1, 1),
  vec4(1, -1, 1, 1),
  vec4(1, -1, -1, 1),
  vec4(-1, -1, -1, 1),
  vec4(0, 0, 0, 1)
);

vec4 cube_normals[42] = vec4[42](
  //front
  vec4(0, 0, -1, 1),
  vec4(0, 0, -1, 1),
  vec4(0, 0, -1, 1),
  vec4(0, 0, -1, 1),
  vec4(0, 0, -1, 1),
  vec4(0, 0, -1, 1),
  vec4(0, 0, 0, 1),
  // back
  vec4(0, 0, 1, 1),
  vec4(0, 0, 1, 1),
  vec4(0, 0, 1, 1),
  vec4(0, 0, 1, 1),
  vec4(0, 0, 1, 1),
  vec4(0, 0, 1, 1),
  vec4(0, 0, 0, 1),
  // left
  vec4(-1, 0, 0, 1),
  vec4(-1, 0, 0, 1),
  vec4(-1, 0, 0, 1),
  vec4(-1, 0, 0, 1),
  vec4(-1, 0, 0, 1),
  vec4(-1, 0, 0, 1),
  vec4(0, 0, 0, 1),
  // right
  vec4(1, 0, 0, 1),
  vec4(1, 0, 0, 1),
  vec4(1, 0, 0, 1),
  vec4(1, 0, 0, 1),
  vec4(1, 0, 0, 1),
  vec4(1, 0, 0, 1),
  vec4(0, 0, 0, 1),
  // top
  vec4(0, 1, 0, 1),
  vec4(0, 1, 0, 1),
  vec4(0, 1, 0, 1),
  vec4(0, 1, 0, 1),
  vec4(0, 1, 0, 1),
  vec4(0, 1, 0, 1),
  vec4(0, 0, 0, 1),
  // bottom
  vec4(0, -1, 0, 1),
  vec4(0, -1, 0, 1),
  vec4(0, -1, 0, 1),
  vec4(0, -1, 0, 1),
  vec4(0, -1, 0, 1),
  vec4(0, -1, 0, 1),
  vec4(0, 0, 0, 1)
);

void main() {
  float cube_size_m = cube_size - 0.0001;
  for (int x = 0; x < 42; x++) {
    gl_Position = gl_in[0].gl_Position + (perspective_matrix * cam_matrix * cube_triangles[x] * (cube_size_m / 2));
    ofu.pos = gl_Position;
    ofu.normal = normalize(
        perspective_matrix * cam_rot_matrix * cube_normals[x]);
    o.rand = i[0].rand;
    EmitVertex();
  }
}
