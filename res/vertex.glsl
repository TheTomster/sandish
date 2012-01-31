#version 150

const float camera_near = 0.1;
const float camera_far = 30.0;

in vec4 position;

uniform float aspect_ratio;

out geo {
  float depth;
} o;

void main() {
  vec4 dev_space;
  dev_space.x = position.x * aspect_ratio;
  dev_space.y = position.y;
  dev_space.z =
      position.z * (camera_near + camera_far) / (camera_near - camera_far);
  dev_space.z += 2.0 * camera_near * camera_far / (camera_near - camera_far);
  dev_space.w = -position.z;

  gl_Position = dev_space;
  o.depth = position.z;
}
