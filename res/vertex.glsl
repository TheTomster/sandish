#version 150

in vec4 position;

uniform mat4 cam_matrix;
uniform mat4 perspective_matrix;

out geo {
  float rand;
} o;

void main() {
  // Generate random noise
  // One-line RNG from
  // http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
  float a = position.x * position.z;
  float b = position.y * position.z;
  float r = fract(sin(dot(vec2(a, b), vec2(12.9898, 78.233))) * 43758.5453);

  gl_Position = perspective_matrix * cam_matrix * position;
  o.rand = r;
}
