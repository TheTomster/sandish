#version 150

in frag {
  float rand;
} i;

void main() {
  float intensity = 0.5 + (0.5 * i.rand);
  gl_FragColor = vec4(intensity, intensity, 0, 1.0f);
}
